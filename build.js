var taskman = require('./tools/taskman');
var fs = require('fs');
var path = require('path');
var isWindows = process.platform === 'win32';
var args = taskman.args;

function print_help(exit_code) {
  var params = [
    {option: "--help", text: "display available options"},
    {option: "", text: ""}, // placeholder
    {option: "--cid_node=[commit-id]", text: "Checkout a particular commit from node/node-chakracore repo"},
    {option: "--cid_jxcore=[commit-id]", text: "Checkout a particular commit from jxcore repo"},
    {option: "--url_node=[node repo url]", text: "URL for node.js Github repo"},
    {option: "--url_jxcore=[jx repo url]", text: "URL for jxcore Github repo"},
    {option: "--dest-cpu=[cpu_type]", text: "set target cpu (arm, ia32, x86, x64). i.e. --dest-cpu=ia32"},
    {option: "--ndk-path=[path]", text: "path to android ndk. This option is required for platform=android"},
    {option: "--platform=[target]", text: "set target platform. by default 'desktop'. (android, desktop, ios, windows-arm)"},
    {option: "--force-target=[jxcore or nodejs]", text: "Force target framework regardless from the platform"},
    {option: "--release", text: "Build release binary. By default Debug"},
    {option: "--reset", text: "Clone nodejs and jxcore repos once again"},
    {option: "--test", text: "run tests after build"}
  ];
  for(var i in params) {
    var param = params[i];
    var space = new Buffer(28 - param.option.length); space.fill(" ");
    console.log(param.option, space + ": ", param.text);
  }
  process.exit(exit_code);
}

if (args.hasOwnProperty('--help')) {
  print_help(0);
} else {
  console.log('tip: try "--help" for other options');
}

var platform = args.hasOwnProperty('--platform') ? args['--platform'] : 'desktop';
var forced_target = args['--force-target'];
var release = args.hasOwnProperty('--release');

function createScript() {
  var cpu = "";
  if (!isWindows) {
    if (args.hasOwnProperty('--dest-cpu')) {
      cpu = "--dest-cpu=" + args['--dest-cpu'];
    } else {
      cpu = "--dest-cpu=ia32"
    }

    if (platform == "ios" || platform == "android" || forced_target == 'jxcore') {
      var script = 'cd jxcore\n';
      if (platform == "android") {
        if (!args.hasOwnProperty('--ndk-path')) {
          console.error('forget "--ndk-path" ??\n');
          print_help(1);
        }

        forced_target = 'jxcore';
        script += "build_scripts/android-configure.sh " + args["--ndk-path"] + "\n"
              + "if [ $? != 0 ]; then\n"
              + "  exit 1\n"
              + "fi\n"
              + "build_scripts/android_compile.sh " + args["--ndk-path"] + "\n"
              + "if [ $? != 0 ]; then\n"
              + "  exit 1\n"
              + "fi\n";
        return script;
      } else if (platform == "ios") { // ios
        forced_target = 'jxcore';
        script += "build_scripts/ios_compile.sh\n"
                + "if [ $? != 0 ]; then\n"
                + "  exit 1\n"
                + "fi\n";
        return script;
      } else {
        forced_target = 'jxcore';
        return "cd jxcore\n./configure --engine-mozilla --static-library "
                + (release ? ' ' : '--debug ') + cpu
                + '\nmake';
      }
    } else {
      forced_target = 'nodejs';
      return 'cd nodejs;./configure --enable-static '
            + (release ? ' ' : '--debug ') + cpu
            + ';make -j ' + require('os').cpus().length;
    }
  } else {
    if (platform == "windows-arm") {
      cpu = "arm";
    } else if (args.hasOwnProperty('--dest-cpu')){
      cpu = args['--dest-cpu'];
    } else {
      cpu = 'ia32';
    }

    if (platform == "windows-arm" && forced_target == 'jxcore') {
      console.error("For Windows ARM, node-chakracore is the only supported option.");
      exit(1);
    }

    var batch = "";
    if (forced_target == 'jxcore')
      batch += 'cd jxcore\nvcbuild.bat ia32 --shared-library ';
    else {
      forced_target = 'nodejs';
      batch += 'cd nodejs\nvcbuild.bat chakracore nosign ' + cpu + ' ';
    }

    batch += (release ? 'release ' : 'debug ') + '\n'
          + 'set EXIT_CODE=%errorlevel%\n'
          + 'cd ..\n'
          + 'exit /b %EXIT_CODE%\n';

    return batch;
  }
}

var stash = function(repo) {
  return path.join(__dirname, "temp/clean.bat") + " " + repo;
};

var compile = function(repo) {
  return path.join(__dirname, "temp/compile.bat") + " " + repo;
};

var createBatch = function() {
  try {
    fs.mkdirSync('./temp');
  } catch(e) { }

  // this needs to be called first
  fs.writeFileSync('./temp/compile.bat', createScript());

  if (isWindows) {
    fs.writeFileSync('./temp/clean.bat', 'cd %1\ngit checkout -f');
    fs.writeFileSync('./temp/copy.bat',  ('del $TAPP*.lib \n'
                                       + 'del $TAPP*.dll \n'
                                       + 'del $TAPP*.exe \n'
                                       + 'copy $$TARGET\\$$MODE\\*.lib $TAPP\n'
                                       + 'copy $$TARGET\\$$MODE\\*.dll $TAPP\n')
                                       .replace(/\$\$MODE/g, release ? "Release" : "Debug")
                                       .replace(/\$TAPP/g, path.join(__dirname, 'winproj\\test_app/'))
                                       .replace(/\$\$TARGET/g, path.join(__dirname, forced_target)));
  } else {
    fs.writeFileSync('./temp/clean.bat', 'cd $1;git checkout -f');
  }

  var br_node = args.hasOwnProperty('--cid_node') ? args['--cid_node'] : "master";
  var br_jxcore = args.hasOwnProperty('--cid_jxcore') ? args['--cid_jxcore'] : "master";
  fs.writeFileSync('./temp/checkout_node.bat', taskman.checkout('nodejs', br_node));
  fs.writeFileSync('./temp/checkout_jxcore.bat', taskman.checkout('jxcore', br_jxcore));

  if (!isWindows) {
    fs.writeFileSync('./temp/test.bat', 'cd tests\nnode runtests.js --target=' + forced_target
                                      + ' --dest-cpu=' + (args.hasOwnProperty('--dest-cpu') ? args['--dest-cpu'] : 'ia32')
                                      + '\nexit $?');
  } else {
    fs.writeFileSync('./temp/test.bat', 'cd tests\nnode runtests.js --target=' + forced_target
                                      + ' --dest-cpu=' + (args.hasOwnProperty('--dest-cpu') ? args['--dest-cpu'] : 'ia32')
                                      + '\nexit %errorlevel%');
  }

  if (!isWindows) {
    fs.chmodSync('./temp/test.bat', '0755');
    fs.chmodSync('./temp/clean.bat', '0755');
    fs.chmodSync('./temp/compile.bat', '0755');
    fs.chmodSync('./temp/checkout_node.bat', '0755');
    fs.chmodSync('./temp/checkout_jxcore.bat', '0755');
  }
}

var setup = function() {
  var node_url = 'https://github.com/' + (isWindows ? 'nodejs/node-chakracore' : 'nodejs/node', 'nodejs');
  if (args.hasOwnProperty('--url_node')) {
    node_url = args['--url_node'];
  }

  var jxcore_url = 'https://github.com/jxcore/jxcore';
  if (args.hasOwnProperty('--url_jxcore')) {
    jxcore_url = args['--url_jxcore'];
  }

  taskman.tasker = [
    [taskman.rmdir('nodejs'), "deleting nodejs folder"],
    [taskman.rmdir('jxcore'), "deleting jxcore folder"],
    [taskman.clone(node_url, 'nodejs'), "cloning nodejs"],
    [taskman.clone(jxcore_url, 'jxcore'), "cloning jxcore"],
    [path.join(__dirname, 'temp/checkout_node.bat'), "checkout nodejs branch"],
    [path.join(__dirname, 'temp/checkout_jxcore.bat'), "checkout jxcore branch"]
  ];
};

if (args.hasOwnProperty('--reset') || !fs.existsSync(path.resolve('nodejs'))) {
  console.log("Setting Up! [ This will take some time.. ]");
  setup();
} else {
  taskman.tasker = [
    [stash('nodejs'), "resetting nodejs folder"],
    [stash('jxcore'), "resetting jxcore folder"]
  ];
}

var patch_nodejs = function() {
  console.log("[ patching node.gyp ]");
  {
    var node_gyp = fs.readFileSync('./nodejs/node.gyp') + "";

    var len_gyp = node_gyp.length;
    // add patch source files
    // todo: make this regex
    node_gyp = node_gyp.replace("'sources': [",
      "'sources': [\n'../patch/node/node_vfile.cc',\n'../patch/node/node_wrapper.cc',\n");

    if (node_gyp.length == len_gyp) {
      console.error('FATAL ERROR: could not patch node.gyp with cc files');
      process.exit(1);
    }
    if (isWindows) {
      node_gyp = node_gyp.replace(/shared_library/g, 'loadable_module');
      len_gyp = node_gyp.length;

      // todo: make this regex
      node_gyp = node_gyp.replace("[ 'node_shared==\"false\"', {",
          "['node_shared==\"true\"',{'msvs_settings':"
        + "{'VCCLCompilerTool':{'RuntimeLibrary': 3}}}],\n"
        + "[ 'node_shared==\"false\"', {");

      if (node_gyp.length == len_gyp) {
        console.error('FATAL ERROR: could not patch node.gyp with MSVS settings');
        process.exit(1);
      }
      len_gyp = node_gyp.length;

      node_gyp = node_gyp.replace(/node_module_version!=\"\"/g,
                  'node_module_version!="" and OS!="win"');
    }

    fs.writeFileSync('./nodejs/node.gyp', node_gyp);
  }

  if (isWindows) {
    console.log("nodejs -> [ patching configure ]");
    {
      var node_configure = fs.readFileSync('./nodejs/configure') + "";
      fs.writeFileSync('./nodejs/configure',
          node_configure.replace('b(options.shared)',"'true'"));
    }
  }

  console.log("nodejs -> [ patching node.cc ]");
  {
    var nodejs_cc = fs.readFileSync('./nodejs/src/node.cc') + "";

    for(var n = nodejs_cc.length-1;n > 0; n--) {
      var ch = nodejs_cc[n];
      if (ch == '}') { // find namespace closing
        var left = nodejs_cc.substr(0, n);
        var right = nodejs_cc.substr(n);
        fs.writeFileSync('./nodejs/src/node.cc', left
              + '\n#include "../../patch/node/node_internal_wrapper.cc"\n' + right);
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

var patch_jxcore = function() {
  console.log("[ patching jx.gyp ]")
  var node_gyp = fs.readFileSync('./jxcore/jx.gyp') + "";
  var len = node_gyp.length;
  node_gyp = node_gyp.replace("'sources': [", "'sources': [\n'../patch/jxcore/jxcore_wrapper.cc',\n");
  if (len == node_gyp.length) {
    console.error("FAILED to patch jx.gyp");
    process.exit(1);
  }
  fs.writeFileSync('./jxcore/jx.gyp', node_gyp);
};

var finalize = function() {
  console.log('done.');
};

var compiled_script_path = compile(forced_target);
// create batch files
createBatch();

// tasker will be executing each one of them one by one
// if any of these actions fail, execution will stop
taskman.tasker.push(
  [patch_nodejs],
  [patch_jxcore],
  [compiled_script_path, "building for " + forced_target + "@" + platform],
  [isWindows ? path.join(__dirname, "temp/copy.bat") : null, "copying Windows binaries"],
  [args.hasOwnProperty('--test') ? path.join(__dirname, "temp/test.bat") : null, "testing"],
  [finalize]
);

taskman.runTasks();
