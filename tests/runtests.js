var taskman = require('../tools/taskman');
var fs = require('fs');
var path = require('path');
var isWindows = process.platform === 'win32';
var args = taskman.args;

var build_type = args.hasOwnProperty('--mode') ? args['--mode'] : "Debug";
var root_folder = path.join(__dirname, "../");

function print_help(exit_code) {
  var params = [
    {option: "--help", text: "display available options"},
    {option: "", text: ""}, // placeholder
    {option: "--file=[test name]", text: "Test only the given single test"},
    {option: "--mode=[build mode]", text: "Set Debug, or Release. Default `Debug`"},
    {option: "--target=[target binary]", text: "Set jxcore, or nodejs. [Required]"}
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
  if (!args.hasOwnProperty('--target')) {
    console.error("missing required `--target` argument\n");
    print_help(1);
  }
  console.log('tip: try "--help" for other options');
}

function build() {
  if (!isWindows) {
    return "cd " + root_folder + "tests\n"
         + "echo Testing $$TARGET_TEST\n"
         + "make BUILD_TYPE=" + build_type + " TARGET_TEST=$$TARGET_TEST TEST_BINARY=$$TEST_BINARY\n"
         + "OUT=$?\n"
         + "if [ $OUT == 0 ]; then\n"
         + "  ./test.o\n"
         + "  OUT=$?\n"
         + "fi\n"
         + "rm -rf test.o\n"
         + "exit $OUT\n"
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

script = script.replace(/\$\$TEST_BINARY/g, args['--target']);

var test_script = path.join(root_folder, "temp/build_test.bat");
fs.writeFileSync(test_script, script);

if (!isWindows) {
  fs.chmodSync(test_script, "0755");
}

taskman.tasker = [
  [test_script, "Testing.." ],
  [function() { console.log('done.'); }]
];

taskman.runTasks();
