package com.remotecontrol;

class Layout {

    int id;
    private int[] parameters;

    Layout(int id) {
        this.id = id;
    }

    Layout(int id, int[] parameters) {
        this.id = id;
        this.parameters = parameters;
    }

    void onMount() {}

    void onUnmount() {}
}
