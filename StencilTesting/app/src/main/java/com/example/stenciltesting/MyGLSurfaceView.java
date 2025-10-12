package com.example.stenciltesting;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import androidx.annotation.NonNull;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends GLSurfaceView implements ScaleGestureDetector.OnScaleGestureListener {
    private static final String TAG = "MyGLSurfaceView";
    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;

    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;
    public static final int IMAGE_FORMAT_YUYV = 0x05;
    public static final int IMAGE_FORMAT_GARY = 0x06;

    public static final int IMAGE_FORMAT_I444 = 0x07;

    private MyNativeRender mNativeRender;
    private MyGLRender myGLRender;

    private int mRatioWidth = 0;
    private int mRatioHeight = 0;

    private long mLastMultiTouchTime;

    private float mPreviousY;
    private float mPreviousX;

    private ScaleGestureDetector mScaleGestureDetector;

    private float mPreScale = 1.0f;
    private float mCurScale = 1.0f;

    private int mXAngle;
    private int mYAngle;

    public MyGLSurfaceView(Context context) {
        this(context, null);
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.setEGLContextClientVersion(3);
        mNativeRender = new MyNativeRender();
        myGLRender = new MyGLRender(mNativeRender);
        setEGLConfigChooser(8, 8, 8, 8, 16, 8);
        setRenderer(myGLRender);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
        mScaleGestureDetector = new ScaleGestureDetector(context, this);
    }

    public void setAspectRatio(int width, int height) {
        mRatioWidth = width;
        mRatioHeight = height;
        requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (mRatioWidth == 0 || mRatioHeight == 0) {
            setMeasuredDimension(width, height);
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getPointerCount() == 1) {
            long currentTimeMillis = System.currentTimeMillis();
            if (currentTimeMillis - mLastMultiTouchTime > 200) {
                float x = event.getX();
                float y = event.getY();

                switch (event.getAction()) {
                    case MotionEvent.ACTION_MOVE:
                        float dy = y - mPreviousY;
                        float dx = x - mPreviousX;

                        mYAngle += dx * TOUCH_SCALE_FACTOR;
                        mXAngle += dy * TOUCH_SCALE_FACTOR;
                }

                mPreviousY = y;
                mPreviousX = x;

                mNativeRender.native_UpdateTransformMatrix(mXAngle, mYAngle, mCurScale, mCurScale);
                requestRender();
            }
            return true;
        } else {
            mScaleGestureDetector.onTouchEvent(event);
        }

        return true;
    }

    public MyNativeRender getNativeRender() {
        return mNativeRender;
    }

    @Override
    public boolean onScale(@NonNull ScaleGestureDetector detector) {
        float preSpan = detector.getPreviousSpan();
        float curSpan = detector.getCurrentSpan();

        if (curSpan < preSpan) {
            mCurScale = mPreScale - (preSpan-curSpan) / 200;
        } else {
            mCurScale = mPreScale + (curSpan-preSpan) / 200;
        }

        mCurScale = Math.max(0.05f, Math.min(mCurScale, 80.0f));

        mNativeRender.native_UpdateTransformMatrix(mXAngle, mYAngle, mCurScale, mCurScale);
        requestRender();
        return false;
    }

    @Override
    public boolean onScaleBegin(@NonNull ScaleGestureDetector detector) {
        return true;
    }

    @Override
    public void onScaleEnd(@NonNull ScaleGestureDetector detector) {
        mPreScale = mCurScale;
        mLastMultiTouchTime = System.currentTimeMillis();
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
//            Log.i(TAG, "onDrawFrame gl = " + gl);
            mNativeRender.native_OnDrawFrame();
        }
    }
}
