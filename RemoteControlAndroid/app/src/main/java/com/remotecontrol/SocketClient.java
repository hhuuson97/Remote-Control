package com.remotecontrol;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

class SocketClient implements Runnable {

    private Thread thread;
    private String key;
    private MainActivity mainActivity;
    private Socket mSocket;
    private PrintStream printStream;
    private DataInputStream dataInputStream;

    @Override
    public void finalize() {
        try {
            if (mSocket != null)
                mSocket.close();
            if (printStream != null)
                printStream.close();
            if (dataInputStream != null)
                dataInputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void send(String title, String description) {
        if (printStream != null)
            new SocketSendThread(printStream,title + " " + description).execute();
    }

    void recv(MainActivity activity) {
        if (dataInputStream != null)
            new SocketReceiveThread(dataInputStream, activity).start();
    }

    @Override
    public void run() {
        try {
            mSocket = new Socket(key, 3000);
            printStream = new PrintStream(mSocket.getOutputStream());
            dataInputStream = new DataInputStream(mSocket.getInputStream());
            send("login", key);
            recv(mainActivity);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void start(String key, MainActivity activity) {
        this.key = key;
        this.mainActivity = activity;
        thread = new Thread(this);
        thread.start();
    }

    synchronized void close() {
        try {
            if (mSocket != null)
                mSocket.close();
            if (dataInputStream != null)
                dataInputStream.close();
            if (printStream != null)
                printStream.close();
            mSocket = null;
            dataInputStream = null;
            printStream = null;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
