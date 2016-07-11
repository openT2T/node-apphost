var exec_ = require('child_process').exec;
var fs = require('fs');
var path = require('path');
var eopts = {
  encoding: 'utf8', timeout: 0,
  maxBuffer: 1e8, killSignal: 'SIGTERM'
};

var isWindows = process.platform === 'win32';
var RMDIR = function(f) {
  if (fs.existsSync(f)) {
    return (isWindows ? "RD /S /Q " : "rm -rf ") + path.resolve(f);
  }
};

function createScript() {
  var cpu = "";
  if (args.hasOwnProperty('--dest-cpu')) {
    cpu = args['--dest-cpu'];
  } else if (args.hasOwnProperty('x86')) {
    cpu = "--dest-cpu=x86";
  } else if (args.hasOwnProperty('x64')) {
    cpu = "--dest-cpu=x64";
  } else if (args.hasOwnProperty('arm')) {
    cpu = "--dest-cpu=arm";
  } else if (args.hasOwnProperty('ia32')) {
    cpu = "--dest-cpu=ia32";
  }

  if (!isWindows) {
    return 'cd $1;./configure --enable-static ' + cpu + ';make -j ' + require('os').cpus().length;
  } else {
    return 'cd %1\nvcbuild.bat chakracore ' + cpu;
  }
}

var createBatch = function() {
  if (!fs.existsSync('temp')) {
    fs.mkdirSync('./temp');

    if (isWindows) {
      fs.writeFileSync('./temp/stash.bat', 'cd %1\ngit stash\ngit stash clear');
    } else {
      fs.writeFileSync('./temp/stash.bat', 'cd $1;git stash;git stash clear');
    }

    fs.writeFileSync('./temp/compile.bat', createScript());

    if (!isWindows) {
      fs.chmodSync('./temp/stash.bat', '0755');
      fs.chmodSync('./temp/compile.bat', '0755');
    }
  }
}

var CLONE = function(repo, target) {
  return "git clone https://github.com/" + repo + " " + target;
};

var exec = function(cmd, callback) {
  return exec_(cmd, eopts, function(err, stdout, stderr) {
    if (err) {
      console.error(err);
      console.error("FAILED:", cmd);
      process.exit(1);
    } else {
      callback(stdout);
    }
  });
};

var STASH = function(repo) {
  return path.join(__dirname, "temp/stash.bat") + " " + repo;
};

var COMPILE = function(repo) {
  return path.join(__dirname, "temp/compile.bat") + " " + repo;
};

var tasker;
var setup = function() {
  tasker = [
    [RMDIR('nodejs'), "deleting nodejs folder"],
    [RMDIR('jxcore'), "deleting jxcore folder"],
    [CLONE(isWindows ? 'nodejs/node-chakracore' : 'nodejs/node', 'nodejs'), "cloning nodejs"],
    [CLONE('jxcore/jxcore', 'jxcore'), "cloning jxcore"]
  ];
};

var args = {};
for(var i = 2; i < process.argv.length; i++) {
  var arg = process.argv[i];
  var a = arg.split('=');
  args[a[0]] = a.length > 1 ? a[1] : 1;
}

if (args.hasOwnProperty('--reset') || !fs.existsSync(path.resolve('nodejs'))) {
  console.log("Setting Up! [ This will take some time.. ]");
  setup();
} else {
  tasker = [
    [STASH('nodejs'), "resetting nodejs source codes"],
    [STASH('jxcore'), "resetting jxcore source codes"]
  ];
}

var patch_nodejs = function() {
  console.log("[ patching node.gyp ]")
  var node_gyp = fs.readFileSync('./nodejs/node.gyp') + "";
  node_gyp = node_gyp.replace("'sources': [\n", "'sources': [\n'../patch/node/node_vfile.cc',\n'../patch/node/node_wrapper.cc',\n");
  fs.writeFileSync('./nodejs/node.gyp', node_gyp);

  console.log("nodejs -> [ patching node.cc ]");
  {
    var nodejs_cc = fs.readFileSync('./nodejs/src/node.cc') + "";

    for(var n = nodejs_cc.length-1;n > 0; n--) {
      var ch = nodejs_cc[n];
      if (ch == '}') { // find namespace closing
        var left = nodejs_cc.substr(0, n);
        var right = nodejs_cc.substr(n);
        fs.writeFileSync('./nodejs/src/node.cc', left + '\n#include "../../patch/node/node_internal_wrapper.cc"\n' + right);
        break;
      }
    }
  }

  console.log("nodejs -> [ patching module.js ]");
  {
    var module_patch = fs.readFileSync('patch/node/module_patch.js') + "";
    var module_js = fs.readFileSync('./nodejs/lib/module.js') + "";
    fs.writeFileSync('./nodejs/lib/module.js', module_js + "\n" + module_patch);
  }

  console.log("nodejs -> [ patching fs.js ]");
  {
    var fs_patch = fs.readFileSync('patch/node/fs_patch.js') + "";
    var fs_js = fs.readFileSync('./nodejs/lib/fs.js') + "";
    fs.writeFileSync('./nodejs/lib/fs.js', fs_js + "\n" + fs_patch);
  }
};

var finalize = function() {
  console.log('done.');
};

tasker.push(
  [patch_nodejs],
  [COMPILE('nodejs'), "building nodejs"],
  [finalize]
);

var runTasks = function() {
  if (!tasker.length) return;

  var task = tasker.shift();
  if (task[0] && task[1]) {
    console.log("Running [", task[1], "]");
    task = task[0];
    exec(task, function(callback){
      if (tasker.length) {
        runTasks();
      }
    });
  } else {
    if (task[0]) {
      task[0]();
    }
    setTimeout(runTasks, 0);
  }
};

createBatch();
runTasks();
