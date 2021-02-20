[![](https://jitpack.io/v/JokChen/x264.svg)](https://jitpack.io/#JokChen/x264)

# x264
该项目用于将yuv420p视屏数据编码成H264数据

### 添加依赖
```groovy
implementation 'com.github.JokChen:x264:1.0.6'
```

### 添加混淆规则

```
-dontwarn com.jokchen.x264.encoder.**
-keep class com.jokchen.x264.encoder.*{*;}
```
