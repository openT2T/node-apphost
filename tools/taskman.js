var exec_ = require('child_process').exec;
var fs = require('fs');
var path = require('path');
var eopts = {
  encoding: 'utf8', timeout: 0,
  maxBuffer: 1e8, killSignal: 'SIGTERM'
};

var isWindows = process.platform === 'win32';
var args = {};
for(var i = 2; i < process.argv.length; i++) {
  var arg = process.argv[i];
  var a = arg.split('=');
  args[a[0].replace(/"/g, "")] = a.length > 1 ? a[1] : 1;
}

exports.args = args;

exports.rmdir = function(f) {
  if (fs.existsSync(f)) {
    return (isWindows ? "RD /S /Q " : "rm -rf ") + path.resolve(f);
  }
};

exports.clone = function(repo, target) {
  return "git clone https://github.com/" + repo + " " + target;
};

exports.checkout = function(repo, id) {
  return "cd " + repo + "\ngit checkout " + id;
}

exports.exec = function(cmd, callback) {
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

exports.tasker = [];
exports.runTasks = function() {
  if (!exports.tasker.length) return;

  var task = exports.tasker.shift();
  if (task[0] && task[1]) {
    console.log("Running [", task[1], "]");
    task = task[0];
    exports.exec(task, function(callback){
      if (exports.tasker.length) {
        exports.runTasks();
      }
    });
  } else {
    if (task[0]) {
      task[0]();
    }
    setTimeout(exports.runTasks, 0);
  }
};
