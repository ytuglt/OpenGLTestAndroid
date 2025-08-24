package com.example.opengluvdemo;

public class MyNativeRender {
    // Used to load the 'opengl' library on application startup.
    static {
        System.loadLibrary("opengltriangledemo");
    }

    public native void native_OnInit();

    public native void native_OnUnInit();

    public native void native_SetImageData(int format, int width, int height, byte[] bytes);

    public native void native_OnSurfaceCreated();

    public native void native_OnSurfaceChanged(int width, int height);

    public native void native_OnDrawFrame();

}
