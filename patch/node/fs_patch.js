var orig_statSync = fs.statSync;
var orig_realpathSync = fs.realpathSync;
var orig_readFileSync = fs.readFileSync;

function REPLACE_NODE_API() {
  var CLEAN = function(file) {
    while (file && (file[0] == pathModule.sep || file[0] == '?'))
      file = file.substr(1);
    return file;
  };

  var vfile = process.binding("node_wrap_vfile");
  var GetVFile = function(path, get_contents) {
    var file = CLEAN(pathModule.relative(process.execPath, path));
    var res = vfile.getFileData(file, get_contents);
    if (!res) {
      file = CLEAN(pathModule.relative(process.cwd(), path));
      res = vfile.getFileData(file, get_contents);
      if (!res) {
        file = CLEAN(path.replace(process.cwd(), ""));
        res = vfile.getFileData(file, get_contents);
      }
    }
    return res;
  };

  fs.statSync = function(path) {
    var file_data = GetVFile(path);
    if (file_data) {
      var statSample = orig_statSync(process.execPath);
      statSample.ino = file_data.ino;
      statSample.size = file_data.size;
      return statSample;
    }

    return orig_statSync(path);
  };

  fs.realpathSync = function(path, options) {
    var file_data = GetVFile(path);
    if (file_data) {
      return path;
    }

    return orig_statSync(path, options);
  };

  fs.readFileSync = function(path, options) {
    var file_data = GetVFile(path,true);
    if (file_data) {
      return file_data.contents;
    }

    return orig_readFileSync(path, options);
  };
}
REPLACE_NODE_API();
