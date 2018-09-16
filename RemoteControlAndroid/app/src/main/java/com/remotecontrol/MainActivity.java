package com.remotecontrol;

import android.annotation.SuppressLint;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    String[] settingArray = {"Set window view", "Set update time", "Turn off computer",
            "Transfer file", "Run Command Prompt", "Run Process", "Kill process", "Exit"};
    String[][] keyBoard = {
            {
                "ESC", "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BSP"
            },
            {
                "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "\\"
            },
            {
                "CAPS", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "ENTER"
            },
            {
                "SHF", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "SHF"
            },
            {
                "CTRL", "WIN", "ALT", "SPACE", "ALT", "WIN", "CTRL", "<", "^", "v", ">"
            }
    };
    String[][] keyCode = {
            {
                "27", "192", "49", "50", "51", "52", "53", "54", "55", "56", "57", "48", "189", "187", "8"
            },
            {
                "9", "81", "87", "69", "82", "84", "89", "85", "73", "79", "80", "219", "221", "220"
            },
            {
                "20", "65", "83", "68", "70", "71", "72", "74", "75", "76", "186", "222", "13"
            },
            {
                "160", "90", "88", "67", "86", "66", "78", "77", "188", "190", "191", "161"
            },
            {
                "162", "164", "18", "32", "18", "165", "163", "37", "38", "40", "39"
            }
    };
    String[] runAppName = { "Calculator", "Internet Explorer", "Notepad" };
    String[] runAppCmd = { "calc", "iexplore", "notepad" };
    Process[] processArray = {};
    String[] windowArray = {};

    ArrayAdapter adapter, adapter1;

    LayoutManager layoutManager;
    Layout layoutLogin, layoutMain, layoutMainHorizonal, layoutKeyBoard, layoutSetting, layoutKillProcess,
        layoutKillProcessOption, layoutRunProcess, layoutCmd, layoutTransferFile,
        layoutWindowView, layoutUpdateTime, layoutTurnOffComputer;

    Layout[] settingLayoutList;

    SocketClient socket;

    public MainActivity() throws Exception {}

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        socket = new SocketClient();

        layoutLogin = new Layout(R.layout.login) {
            @Override
            void onMount() {
                super.onMount();

                findViewById(R.id.secret_key).setOnKeyListener(new View.OnKeyListener() {
                    public boolean onKey(View v, int keyCode, KeyEvent event) {
                        if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
                                (keyCode == KeyEvent.KEYCODE_ENTER)) {
                            login();
                            return true;
                        }
                        return false;
                    }
                });
            }
        };

        layoutMain = new Layout(R.layout.activity_main) {
            @Override
            void onMount() {
                super.onMount();

                updateActivityMain();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Display display = getWindowManager().getDefaultDisplay();
                        Point size = new Point();
                        display.getSize(size);
                        ImageView imageView = findViewById(R.id.imageView);
                        Drawable bound = imageView.getDrawable();

                        socket.send("viewSize", size.x + " " +
                                size.x * bound.getIntrinsicHeight() / bound.getIntrinsicWidth());
                    }
                });
                ImageView imageView = findViewById(R.id.imageView);
                imageView.setOnTouchListener(new ImageView.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        int[] viewCoords = new int[2];
                        v.getLocationOnScreen(viewCoords);

                        int imageX = (int) event.getX() - viewCoords[0]; // viewCoords[0] is the X coordinate
                        int imageY = (int) event.getY() - viewCoords[1]; // viewCoords[1] is the y coordinate

                        if (event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP)
                            socket.send("mouse", imageX + " " + imageY);
                        return false;
                    }
                });

                updateActivityMain();
                updateMainView();
            }
        };

        layoutMainHorizonal = new Layout(R.layout.activity_main_horizonal) {
            @SuppressLint("ClickableViewAccessibility")
            @Override
            void onMount() {
                super.onMount();

                updateActivityMain();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Display display = getWindowManager().getDefaultDisplay();
                        Point size = new Point();
                        display.getSize(size);
                        ImageView imageView = findViewById(R.id.imageView);
                        Drawable bound = imageView.getDrawable();
                        socket.send("viewSize", size.y * bound.getIntrinsicWidth() / bound.getIntrinsicHeight() + " " +
                                size.y);
                    }
                });
                ImageView imageView = findViewById(R.id.imageView);
                imageView.setOnTouchListener(new ImageView.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        int[] viewCoords = new int[2];
                        v.getLocationOnScreen(viewCoords);

                        int imageX = (int) event.getX() - viewCoords[0]; // viewCoords[0] is the X coordinate
                        int imageY = (int) event.getY() - viewCoords[1]; // viewCoords[1] is the y coordinate

                        if (event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP)
                            socket.send("mouse", imageX + " " + imageY);
                        return false;
                    }
                });
                updateMainView();
            }
        };

        layoutKeyBoard = new Layout(R.layout.keyboard) {
            @Override
            void onMount() {
                super.onMount();

                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                LinearLayout layoutKeyBoard = findViewById(R.id.keyboard);
                LinearLayout line[] = new LinearLayout[5];
                for (int i = 0; i < 5; i++) {
                    line[i] = new LinearLayout(layoutKeyBoard.getContext());
                    line[i].setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                                    LinearLayout.LayoutParams.WRAP_CONTENT, 1));
                    line[i].setOrientation(LinearLayout.HORIZONTAL);
                    for (int j = 0; j < keyBoard[i].length; j++) {
                        Button button = new Button(line[i].getContext());
                        button.setTextSize(8);
                        button.setText(keyBoard[i][j]);
                        button.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT,
                                LinearLayout.LayoutParams.WRAP_CONTENT,1));
                        final String key = keyCode[i][j];
                        button.setOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                socket.send("key", key);
                            }
                        });
                        line[i].addView(button);
                    }
                    layoutKeyBoard.addView(line[i]);
                }
            }

            @Override
            void onUnmount() {
                super.onUnmount();

                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR);
            }
        };

        adapter = new ArrayAdapter<>(this,
                R.layout.item_list_view, settingArray);

        layoutWindowView = new Layout(R.layout.window_view) {
            @Override
            void onMount() {
                super.onMount();

                socket.send("windowList", "");
                setWindowList(windowArray);
            }
        };

        layoutUpdateTime = new Layout(R.layout.update_time);

        layoutTurnOffComputer = new Layout(R.layout.turn_off_computer);

        layoutTransferFile = new Layout(R.layout.transfer_file);

        layoutCmd = new Layout(R.layout.cmd);

        layoutRunProcess = new Layout(R.layout.run_process) {
            @Override
            void onMount() {
                super.onMount();

                setSpinner();
            }
        };

        layoutKillProcess = new Layout(R.layout.kill_process) {
            @Override
            void onMount() {
                super.onMount();

                socket.send("processList", "");
                setKillList(processArray);
            }
        };

        settingLayoutList = new Layout[]{
                layoutWindowView,
                layoutUpdateTime,
                layoutTurnOffComputer,
                layoutTransferFile,
                layoutCmd,
                layoutRunProcess,
                layoutKillProcess
        };

        layoutSetting = new Layout(R.layout.setting) {
            @Override
            void onMount() {
                super.onMount();

                ListView listView = findViewById(R.id.settingList);
                listView.setAdapter(adapter);

                listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        if (position < 7)
                            layoutManager.push(settingLayoutList[position]);
                        else {
                            layoutManager.refresh(layoutLogin);
                            socket.close();
                        }
                    }
                });
            }
        };

        layoutManager = new LayoutManager(this, layoutLogin);
    }

    public void setSpinner() {
        Spinner dropdown = findViewById(R.id.spinner);
        ArrayAdapter<String> adapter = new ArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, runAppName);
        dropdown.setAdapter(adapter);
    }

    public void setWindowList(final String[] array) {
        windowArray = array;
        adapter1 = new ArrayAdapter<>(this, R.layout.item_list_view, windowArray);

        ListView listView1 = findViewById(R.id.window_list_view);
        listView1.setAdapter(adapter1);
        listView1.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
                socket.send("changeWindow", windowArray[position]);
                layoutManager.refresh(layoutMain);
            }
        });
    }

    public void setKillList(final Process[] processArray) {
        String[] processName = new String[processArray.length];
        for (int i = 0; i < processArray.length; i++)
            processName[i] = processArray[i].name;
        adapter1 = new ArrayAdapter<>(this, R.layout.item_list_view, processName);

        ListView listView1 = findViewById(R.id.kill_list_view);
        listView1.setAdapter(adapter1);
        listView1.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
                layoutKillProcessOption = new Layout(R.layout.kill_process_option) {
                    @Override
                    void onMount() {
                        super.onMount();
                        Button btn = findViewById(R.id.button5);
                        btn.setOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                EditText editText = findViewById(R.id.editText2);
                                socket.send("killProcess", processArray[position].id
                                        + " " + editText.getText().toString());
                                Toast.makeText(getApplicationContext(), "Process have already killed",
                                        Toast.LENGTH_SHORT).show();
                                layoutManager.refresh(layoutMain);
                            }
                        });
                    }
                };
                layoutManager.push(layoutKillProcessOption);
            }
        });
    }

    @SuppressLint("ResourceType")
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        updateMainView();
    }

    void updateMainView() {
        if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
            if (layoutManager.layoutList.peek().id == R.layout.activity_main)
                layoutManager.refresh(layoutMainHorizonal);
        } else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
            if (layoutManager.layoutList.peek().id == R.layout.activity_main_horizonal)
                layoutManager.refresh(layoutMain);
        }
    }

    public void updateActivityMain() {
        File root = android.os.Environment.getExternalStorageDirectory();
        File imageFile = new  File(root.getAbsolutePath()+"/Download/screenshot.jpg");
        if (imageFile.exists() && (layoutManager.layoutList.peek().id == R.layout.activity_main ||
                layoutManager.layoutList.peek().id == R.layout.activity_main_horizonal)) {
            ImageView imageView = findViewById(R.id.imageView);
            imageView.setImageBitmap(BitmapFactory.decodeFile(imageFile.getAbsolutePath()));
        }
    }

    public void login() {
        EditText key = findViewById(R.id.secret_key);
        socket.start(key.getText().toString(), this);

        layoutManager.refresh(layoutMain);
        updateMainView();
    }

    public void login(View view) {
        login();
    }

    public void setting(View view) {
        layoutManager.push(layoutSetting);
    }

    public void showKeyboard(View view) {
        layoutManager.push(layoutKeyBoard);
    }

    @SuppressLint("SetTextI18n")
    public void changeState(View view) {
        Button button = findViewById(R.id.button3);
        if (button.getText().equals("Left Mouse"))
            button.setText("Right Mouse");
        else button.setText("Left Mouse");
        socket.send("changeState", "");
    }

    public void updateTime(View view) {
        TextView time = findViewById(R.id.editText7);
        socket.send("updateTime", time.getText().toString());
        Toast.makeText(getApplicationContext(), "Update view time have already updated", Toast.LENGTH_SHORT).show();
        layoutManager.pop();
    }

    public void turnOffComputer(View view) {
        TextView time = findViewById(R.id.editText5);
        socket.send("turnOffComputer", time.getText().toString());
        layoutManager.refresh(layoutMain);
    }

    public void transferFile(View view) {
        TextView path = findViewById(R.id.editText4);
        socket.send("transfer", path.getText().toString());
        Toast.makeText(getApplicationContext(), "File is being downloaded", Toast.LENGTH_SHORT).show();
        layoutManager.pop();
    }

    public void sendCmd(View view) {
        TextView cmdline = findViewById(R.id.editText3);
        socket.send("cmdLine", cmdline.getText().toString());
        layoutManager.refresh(layoutMain);
    }

    public void runProcess(View view) {
        Spinner spinner = findViewById(R.id.spinner);
        socket.send("cmdLine", "start " + runAppCmd[spinner.getSelectedItemPosition()]);
        layoutManager.refresh(layoutMain);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            layoutManager.pop();
            return true;
        }

        return super.onKeyDown(keyCode, event);
    }
}
