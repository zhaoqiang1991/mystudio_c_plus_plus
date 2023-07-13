package com.example.myapplication.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.TextView;

import androidx.annotation.Nullable;

/**
 * @author Lance
 * @date 2018/10/8
 */
public class RecordButton extends androidx.appcompat.widget.AppCompatTextView {


    private static long time;
    private OnRecordListener mListener;

    public RecordButton(Context context) {
        super(context);
    }

    public RecordButton(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mListener == null) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                setPressed(true);
                time = System.currentTimeMillis();
                mListener.onRecordStart();
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                setPressed(false);
                if (is5FastClick()) {
                    return true;
                }
                mListener.onRecordStop();
                break;
        }
        return true;
    }

    public static boolean is5FastClick() {
        long div = time - lastClickTime;
        if (div > 0 && div < 800) {
            countClick += 1;
        } else {
            countClick = 0;
        }
        lastClickTime = time;

        if (countClick >= 5) {
            countClick = 0;
            return true;
        }

        return false;
    }

    private static long lastClickTime;
    private static int countClick = 0;

    public void setOnRecordListener(OnRecordListener listener) {
        mListener = listener;
    }

    public interface OnRecordListener {
        void onRecordStart();

        void onRecordStop();
    }
}
