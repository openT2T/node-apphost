// REPLACE_NODE_API()
function stat(filename) {
  filename = path._makeLong(filename);
  const cache = stat.cache;
  if (cache !== null) {
    const result = cache.get(filename);
    if (result !== undefined) return result;
  }

  try {
    const result = fs.statSync(filename);
    if (cache !== null) cache.set(filename, result);
    return !result ? -2 : result.isDirectory() ? 1 : 0;
  } catch(e) { }
  return -2;
}
