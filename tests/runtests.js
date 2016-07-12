var taskman = require('../tools/taskman');
var fs = require('fs');
var path = require('path');
var isWindows = process.platform === 'win32';
var args = taskman.args;

var build_type = args.hasOwnProperty('release') ? "Release" : "Debug";
var root_folder = path.join(__dirname, "../");

function build() {
  if (!isWindows) {
    return "cd " + root_folder + "tests\necho Testing $$TARGET_TEST\n" 
                 + "make BUILD_TYPE=" + build_type 
                 + " TARGET_TEST=$$TARGET_TEST\nif [ $? == 0 ]; then\n./test.o\n"
                 + "fi\nrm test.o\nif [ $? != 0 ]; then\nexit 1\nfi\n";
  } else {
    return "IMPLEMENT ME";
  }
}

// todo: move me into a json file
var tests = [
  "call-function",
  "create-array",
  "create-object"
];

var script = "";
for(var i=0; i<tests.length; i++) {
  script += build().replace(/\$\$TARGET_TEST/g, tests[i]) + "\n";
}

var test_script = path.join(root_folder, "temp/build_test.bat");
fs.writeFileSync(test_script, script);

if (!isWindows) {
  fs.chmodSync(test_script, "0755");
}

taskman.tasker = [
  [test_script, "Testing.." ],
  function() { console.log('done.'); }
];

taskman.runTasks();
