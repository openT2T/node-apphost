If you are on Windows and have `Visual Studio 2013 and later` version is installed,
the only thing you need to do is building the target framework with `test` argument.

nodejs
```
> node build.js --test --force-target=nodejs
```

jxcore
```
> node build.js --test --force-target=jxcore
```

You should find both `current.sln` and `current.vcxproj` are available under
`winproj/test_app` folder with necessary binary and header files.
