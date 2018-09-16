package com.remotecontrol;

import android.os.AsyncTask;
import java.io.PrintStream;

class SocketSendThread extends AsyncTask<String, Void, String> {

    private PrintStream printStream;
    private String data;

    SocketSendThread(PrintStream printStream, String data) {
        this.printStream = printStream;
        this.data = data;
    }

    @Override
    protected String doInBackground(String... strings) {
        if (printStream != null) {
            printStream.println(data);
        }
        return null;
    }
}
