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
    {option: "--dest-cpu=[cpu_type]", text: "set target cpu (arm, ia32, x86, x64). i.e. --dest-cpu=ia32"},
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

  if (isWindows) {
    // set target on vcxproj file
    var proj_file = path.join(__dirname, "../winproj/test_app/test_app.vcxproj");
    var file = fs.readFileSync(proj_file) + "";
    file = file.replace("$NODE_DISTRO$", (args['--target'] == 'jxcore' ? 'jx' : 'node'));
    file = file.replace(/\$TARGET_ARCH/g, getARCH());
    fs.writeFileSync(path.join(path.dirname(proj_file), "current.vcxproj"), file);
  }
}

function getARCH() {
  var arch = (args.hasOwnProperty('--dest-cpu') ? args['--dest-cpu'] : 'ia32');
  if (!isWindows) {
    if (arch == 'ia32' || arch == 'x86') return '-m32'
    if (arch == 'x64' || arch == 'x86_64' || arch == 'amd64') return '-m64'

    return '-m32';
  } else {
    console.log("IMPLEMENT ME (getARCH) WINDOWS");
  }
}

function build() {
  if (!isWindows) {
    return "cd " + root_folder + "tests\n"
         + "echo Testing $$TARGET_TEST\n"
         + "make BUILD_TYPE=" + build_type
         + " TARGET_TEST=$$TARGET_TEST TEST_BINARY=$$TEST_BINARY "
         + " DEST_CPU=" + getARCH()
         + " PLATFORM=" + process.platform + "\n"
         + "OUT=$?\n"
         + "if [ $OUT == 0 ]; then\n"
         + "  ./test.o\n"
         + "  OUT=$?\n"
         + "fi\n"
         + "rm -rf test.o\n"
         + "if [ $OUT != 0 ]; then\n"
         + "exit $OUT\n"
         + "fi\n";
  } else {
    return "cd " + root_folder + "\\winproj\\test_app\n"
         + "copy ..\\..\\tests\\$$TARGET_TEST\\test.cpp .\n"
         + "msbuild /m /nologo current.sln\n"
         + "if %errorlevel% NEQ 0 exit 1\n"
         + "copy " + (args['--target'] == 'jxcore' ? 'jx' : 'node')
         + ".dll Debug\\\n"
         + "cd Debug\n"
         + "echo *********** RUNNING $$TARGET_TEST *************"
         + "\ncurrent.exe"
         + "\nif %errorlevel% NEQ 0 ("
         + "\nexit 1"
         + "\n)\n";
  }
}

// todo: move me into a json file
var tests = [
  "call-function",
  "check-type",
  "create-array",
  "create-object",
  "native-method",
  "new_value",
  "raise-unhandled",
  "scope-test",
  "stringify-object"
];

// find visual studio environment batch file and execute
// this is needed for msbuild
var script = ""

if (isWindows) {
  script +=  
      "@echo off"
    + "\nset target_env=vc2015"
    + "\n@rem Set environment for msbuild"
    + "\nif defined target_env if \"%target_env%\" NEQ \"vc2015\" goto vc-set-2013"
    + "\n@rem Look for Visual Studio 2015"
    + "\necho Looking for Visual Studio 2015"
    + "\nif not defined VS140COMNTOOLS goto vc-set-2013"
    + "\nif not exist \"%VS140COMNTOOLS%\\..\\..\\vc\\vcvarsall.bat\" goto vc-set-2013"
    + "\necho Found Visual Studio 2015"
    + "\nif \"%VCVARS_VER%\" NEQ \"140\" ("
    + "\n  call \"%VS140COMNTOOLS%\\..\\..\\vc\\vcvarsall.bat\""
    + "\n  SET VCVARS_VER=140"
    + "\n)"
    + "\nif not defined VCINSTALLDIR goto vc-set-2013"
    + "\nset GYP_MSVS_VERSION=2015"
    + "\nset PLATFORM_TOOLSET=v140"
    + "\ngoto msbuild-found"
    + "\n"
    + "\n:vc-set-2013"
    + "\nif defined target_env if \"%target_env%\" NEQ \"vc2013\" goto msbuild-not-found"
    + "\n@rem Look for Visual Studio 2013"
    + "\necho Looking for Visual Studio 2013"
    + "\nif not defined VS120COMNTOOLS goto msbuild-not-found"
    + "\nif not exist \"%VS120COMNTOOLS%\\..\\..\\vc\\vcvarsall.bat\" goto msbuild-not-found"
    + "\necho Found Visual Studio 2013"
    + "\nif \"%VCVARS_VER%\" NEQ \"120\" ("
    + "\n  call \"%VS120COMNTOOLS%\\..\\..\\vc\\vcvarsall.bat\""
    + "\n  SET VCVARS_VER=120"
    + "\n)"
    + "\nif not defined VCINSTALLDIR goto msbuild-not-found"
    + "\nset GYP_MSVS_VERSION=2013"
    + "\nset PLATFORM_TOOLSET=v120"
    + "\ngoto msbuild-found"
    + "\n"
    + "\n:msbuild-not-found"
    + "\necho Failed to find Visual Studio installation."
    + "\nexit 1"
    + "\n"
    + "\n:msbuild-found"
    + "\n";
}

if (!args.hasOwnProperty('--file')) { // test all
  for(var i=0; i<tests.length; i++) {
    script += build().replace(/\$\$TARGET_TEST/g, tests[i]) + "\n";
  }
} else { // single test
  script += build().replace(/\$\$TARGET_TEST/g, args['--file']) + "\n";
}

script = script.replace(/\$\$TEST_BINARY/g, args['--target']);

var test_script = path.join(root_folder, "temp/build_test.bat");
fs.writeFileSync(test_script, script);

if (!isWindows) {
  fs.chmodSync(test_script, "0755");
}

taskman.tasker = [
  [test_script, "Testing.." ],
  [function() { console.log('Tests completed successfully'); }]
];

taskman.runTasks();
