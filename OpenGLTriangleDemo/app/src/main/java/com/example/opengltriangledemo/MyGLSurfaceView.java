package com.example.opengltriangledemo;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends GLSurfaceView {
    private static final String TAG = "MyGLSurfaceView";
    private MyNativeRender mNativeRender;
    private MyGLRender myGLRender;

    public MyGLSurfaceView(Context context) {
        this(context, null);
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.setEGLContextClientVersion(3);
        mNativeRender = new MyNativeRender();
        myGLRender = new MyGLRender(mNativeRender);
        setRenderer(myGLRender);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    public MyNativeRender getNativeRender() {
        return mNativeRender;
    }

    public static class MyGLRender implements Renderer {
        private MyNativeRender mNativeRender;

        public MyGLRender(MyNativeRender nativeRender) {
            mNativeRender = nativeRender;
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.i(TAG, "onSurfaceCreated gl = " + gl + ", config = " + config);
            mNativeRender.native_OnSurfaceCreated();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.i(TAG, "onSurfaceChanged gl = " + gl + ", width = " + width + ", height = " + height);
            mNativeRender.native_OnSurfaceChanged(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            Log.i(TAG, "onDrawFrame gl = " + gl);
            mNativeRender.native_OnDrawFrame();
        }
    }
}
