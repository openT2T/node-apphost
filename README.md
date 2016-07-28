### Requirements

 - `node.js` or `jxcore` is installed
 - For `Windows` Visual Studio 2013 or later versions
 - For `OSX` XCode
 - For `Linux/BSD` gcc/gcc++ and relevant developer packages
 - `git` and `python 2.7` has to be reachable from the path
 - Download and install Android SDK and NDK for Android builds


### Command line options
 ```
 node build.js --help

 --help                     :  display available options
                            :  
 --cid_node=[commit-id]     :  Checkout a particular commit from node/node-chakracore repo
 --cid_jxcore=[commit-id]   :  Checkout a particular commit from jxcore repo
 --url_node=[node repo url] :  URL for node.js Github repository
 --url_jxcore=[jx repo url] :  URL for jxcore Github repository
 --dest-cpu=[cpu_type]      :  set target cpu (arm, ia32, x86, x64). i.e. --dest-cpu=ia32
 --ndk-path=[path]          :  path to android ndk. This option is required for platform=android
 --platform=[target]        :  set target platform. by default 'desktop'. (android, desktop, ios, windows-arm)
 --force-target=[jxcore or nodejs] :  Force target framework regardless from the platform
 --release                  :  Build release binary. By default Debug
 --reset                    :  Clone nodejs and jxcore repos once again
 --test                     :  run tests after build
 ```

### Preparing the Environment

When you call `node build.js` or `jx build.js` for the first time, `TBD` will clone both `nodejs` and `jxcore`
repositories locally. If you want to target a particular commit or branch from either of these repositories,
use one or both of the options below;

```
node build.js --cid_node=xxxxxxxx
```

or

```
node build.js --cid_node=xxxxxxx --cid_jxcore=yyyyy
```

By default, `TBD` clones `node-chakracore` on Windows, and `nodejs/node` for other platforms
You may set the target repository url (it has to be a git repository) using command line below;

```
node build.js --url_node=https://github.com/custom/node_repo
```

Same applies for jxcore using `--url_jxcore` argument.

Once the repositories are downloaded to your local, `build.js` won't be downloading them again.
Even if you target another repository `url` it won't be changing the ones available on local.
In order to force downloading the repositories, you should use `--reset` argument.

### Build and Testing

`TBD` decides which `node distro` will be used based on the target platform. `--platform`
In case you are on desktop (hence targeting desktop), you may force either `nodejs` or `jxcore`
by using `--force-target=jxcore` or `nodejs` similarly. (also see `--url_node` and `--url_jxcore`)

Let's say you are on `Windows`.

```
> node build.js
```

Command above will be patching node.js ChakraCore with necessary wrapper interface and build as a `dll`
You may find resulting `dll` and `header` files under the `out/node_desktop_ia32/` folder.

If you were on `Linux` or `OSX`, command above would patch `nodejs` v8. However, output folder would be the same.

Unless otherwise is specified, `TBD` compiles against `ia32` target. You may use `--dest-cpu=x64` argument for `x64`
For `android`, `ios` and `windows-arm` targets, setting `-dest-cpu` has no effect.

In order to start testing right after build, use below command line;

```
> node build.js --test
```

Forcing `jxcore` target is possible on desktop too.

```
> node build.js --force-target=jxcore --test
```

The command above will patch and build `jxcore` binaries and test them.
Finally, `jxcore` binaries will be available under `out/jxcore_desktop_ia32/`
