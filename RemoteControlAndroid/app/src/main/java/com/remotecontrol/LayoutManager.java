package com.remotecontrol;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.ViewTreeObserver;

import java.util.Stack;

class LayoutManager {

    Stack<Layout> layoutList;
    private AppCompatActivity activity;

    LayoutManager(AppCompatActivity activity ,Layout layout) {
        layoutList = new Stack<>();
        this.activity = activity;
        refresh(layout);
    }

    void refresh(Layout layout) {
        if (layoutList.size() > 0) layoutList.peek().onUnmount();
        layoutList.clear();
        push(layout);
    }

    void push(final Layout layout) {
        layoutList.push(layout);
        activity.setContentView(layout.id);
        layout.onMount();
    }

    void popAndPush(Layout layout) {
        layoutList.peek().onUnmount();
        layoutList.pop();
        push(layout);
    }

    void pop() {
        if (layoutList.size() == 1) {
            Intent homeIntent = new Intent(Intent.ACTION_MAIN);
            homeIntent.addCategory( Intent.CATEGORY_HOME );
            homeIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            activity.startActivity(homeIntent);
        }
        else {
            layoutList.peek().onUnmount();
            layoutList.pop();
            activity.setContentView(layoutList.peek().id);
            layoutList.peek().onMount();
        }
    }
}
