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
                 + "fi\nOUT=0\nif [ $? != 0 ]; then\nOUT=1\nfi\nrm test.o\nexit $OUT";
  } else {
    return "IMPLEMENT ME";
  }
}

// todo: move me into a json file
var tests = [
  "call-function",
  "create-array",
  "create-object",
  "new_value"
];

var script = "";
if (!args.hasOwnProperty('--file')) { // test all
  for(var i=0; i<tests.length; i++) {
    script += build().replace(/\$\$TARGET_TEST/g, tests[i]) + "\n";
  }
} else {
  script = build().replace(/\$\$TARGET_TEST/g, args['--file']) + "\n";
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
