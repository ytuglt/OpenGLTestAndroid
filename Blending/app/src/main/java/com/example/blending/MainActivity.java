package com.example.blending;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;
import static com.example.blending.MyGLSurfaceView.IMAGE_FORMAT_RGBA;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.example.blending.databinding.ActivityMainBinding;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    private MyGLSurfaceView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        glSurfaceView = binding.sampleText;

        glSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
        Bitmap bitmap = loadRGBAImage(R.drawable.board_texture, 0);
        loadRGBAImage(R.drawable.floor, 1);
        loadRGBAImage(R.drawable.window, 2);

//        glSurfaceView.setAspectRatio(bitmap.getWidth(), bitmap.getHeight());
    }


    private Bitmap loadRGBAImage(int resId, int index) {
        InputStream is = this.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                glSurfaceView.getNativeRender().native_SetImageDataWithIndex(index, IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);
            }
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
        return bitmap;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glSurfaceView != null) {
            glSurfaceView.onResume();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (glSurfaceView != null) {
            glSurfaceView.onPause();
        }
    }

}