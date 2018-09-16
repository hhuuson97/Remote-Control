package com.remotecontrol;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.widget.Toast;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class SocketReceiveThread implements Runnable {

    private DataInputStream dataInputStream;
    private MainActivity activity;
    Thread thread;

    SocketReceiveThread(DataInputStream dataInputStream, MainActivity activity) {
        this.dataInputStream = dataInputStream;
        this.activity = activity;
    }

    private boolean checkExternalMedia(){
        boolean mExternalStorageAvailable = false;
        boolean mExternalStorageWriteable = false;
        String state = Environment.getExternalStorageState();

        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // Can read and write the media
            mExternalStorageAvailable = mExternalStorageWriteable = true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            // Can only read the media
            mExternalStorageAvailable = true;
            mExternalStorageWriteable = false;
        } else {
            // Can't read or write
            mExternalStorageAvailable = mExternalStorageWriteable = false;
        }
        return  mExternalStorageAvailable && mExternalStorageWriteable;
    }

    static final int REQUEST_WRITE_STORAGE = 112;

    private void requestPermission(Activity context) {
        boolean hasPermission = (ContextCompat.checkSelfPermission(context, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED);
        if (!hasPermission) {
            ActivityCompat.requestPermissions(context,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    REQUEST_WRITE_STORAGE);
        } else {
            String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/new_folder";
            File storageDir = new File(path);
            if (!storageDir.exists() && !storageDir.mkdirs()) {
                // This should never happen - log handled exception!
            }
        }
    }

    @Override
    synchronized public void run() {
        int type;
        try {
            while (dataInputStream != null) {
                type = readInt32C(dataInputStream);
                switch (type) {
                    case 1: {
                        boolean isDownload = FileDownloader(dataInputStream);
                        if (isDownload)
                            activity.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(activity.getApplicationContext(), "Download completed", Toast.LENGTH_SHORT).show();
                                }
                            });
                        else
                            activity.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(activity.getApplicationContext(), "Error when downloading", Toast.LENGTH_SHORT).show();
                                }
                            });
                        break;
                    }
                    case 2: {
                        final Process[] list = getListProcess(dataInputStream);
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                activity.setKillList(list);
                            }
                        });
                        break;
                    }
                    case 3: {
                        boolean isDownload = FileDownloader(dataInputStream);
                        if (isDownload)
                            activity.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    activity.updateActivityMain();
                                }
                            });
                        break;
                    }
                    case 4: {
                        final String[] list = getListWindow(dataInputStream);
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                activity.setWindowList(list);
                            }
                        });
                        break;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean FileDownloader(DataInputStream dataInputStream) throws IOException {
        final long leng = readInt64C(dataInputStream);
        long count = 0;
        byte []res;
        long glen;

        requestPermission(activity);
        boolean checkWrite = checkExternalMedia();
        String name = readString(dataInputStream);

        File root = android.os.Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + "/Download");
        dir.mkdirs();

        File file = new File(dir, name);
        try (FileOutputStream f = new FileOutputStream(file, false)) {
            res = new byte[102400];
            while (count < leng) {
                glen = dataInputStream.read(res,0, (int) Math.min(leng - count, 102400));
                count += glen;
                f.flush();
                if (checkWrite) f.write(res,0, (int) glen);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return checkWrite;
    }

    private Process[] getListProcess(DataInputStream dataInputStream) throws IOException {
        int leng;
        leng = readInt32C(dataInputStream);
        Process[] list = new Process[leng];

        for (int i = 0; i < leng; i++) {
            String name = readUnicodeString(dataInputStream);
            int id = readInt32C(dataInputStream);
            list[i] = new Process(name, id);
        }
        return list;
    }

    private String[] getListWindow(DataInputStream dataInputStream) throws IOException {
        int leng;
        leng = readInt32C(dataInputStream);
        String[] list = new String[leng];

        for (int i = 0; i < leng; i++) {
            String name = readUnicodeString(dataInputStream);
            list[i] = name;
        }
        return list;
    }

    private int toUnsigned(int x) {
        if (x < 0) return 256 + x;
        return x;
    }

    private int readInt32C(DataInputStream dataInputStream) throws IOException {
        int tmp = 0;
        for (int i = 0; i < 4; i++) {
            long t = toUnsigned(dataInputStream.readByte());
            tmp += t << (i << 3);
        }
        return tmp;
    }

    private long readInt64C(DataInputStream dataInputStream) throws IOException {
        long tmp = 0;
        for (long i = 0; i < 8; i++) {
            long t = toUnsigned(dataInputStream.readByte());
            tmp += t << (i << 3);
        }
        return tmp;
    }

    @NonNull
    private String readUnicodeString(DataInputStream dataInputStream) throws IOException {
        int count = readInt32C(dataInputStream);
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < count; i++) {
            int tmp = toUnsigned(dataInputStream.readByte());
            tmp <<= 8;
            tmp += toUnsigned(dataInputStream.readByte());
            sb.append((char) tmp);
        }
        return sb.toString();
    }

    @NonNull
    private String readString(DataInputStream dataInputStream) throws IOException {
        int count = readInt32C(dataInputStream);
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < count; i++)
            sb.append((char)dataInputStream.readByte());
        return sb.toString();
    }

    void start() {
        thread = new Thread(this);
        thread.start();
    }
}
