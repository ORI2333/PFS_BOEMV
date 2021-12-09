package com.gizwits.opensource.appkit.ControlModule;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class GosCarControlActivity extends GosBaseActivity {
    private Button m_goForwardButton;
    private Button m_turnLeftButton;
    private Button m_StopButton;
    private Button m_turnRightButton;
    private Button m_turnBackButton;
    //private Button m_Going;
    //private Button m_Backing;
    private Button m_cat1;
    private Button m_cat2;
    private Button m_cat3;
    private Button m_cat4;
    private Button m_dog1;
    private Button m_dog2;
    private Button petb;
    private Button m_dog4;
    private Button auto;
    private Button determine;
    private EditText edittext1;
    private EditText edittext2;
    private EditText time1;
    private GizWifiDevice device;
    private Button m_dog3;
    ActionBar actionBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gos_car_control);
        initDevice();
        setActionBar(true,true,"Control");
        initView();
    }

    private void initView() {
        m_goForwardButton = (Button)findViewById(R.id.go_forward_bt);
        m_turnLeftButton = (Button)findViewById(R.id.turn_left_bt);
        m_StopButton = (Button)findViewById(R.id.stop_bt);
        m_turnRightButton = (Button)findViewById(R.id.turn_right_bt);
        m_turnBackButton = (Button)findViewById(R.id.turn_back_bt);
        m_cat1 = (Button)findViewById(R.id.cat_bt1);
        m_cat2 = (Button)findViewById(R.id.cat_bt2);
        m_cat3 = (Button)findViewById(R.id.cat_bt3);
        m_cat4 = (Button)findViewById(R.id.cat_bt4);

        m_dog1 = (Button)findViewById(R.id.dog_bt1);
        m_dog2 = (Button)findViewById(R.id.dog_bt2);
        m_dog3 = (Button)findViewById(R.id.dog_bt3);
        m_dog4 = (Button)findViewById(R.id.dog_bt4);
        petb = (Button)findViewById(R.id.pet_b);
        edittext1 = (EditText)findViewById(R.id.num_text1);
        edittext2 = (EditText)findViewById(R.id.num_text2);
        time1 = (EditText)findViewById(R.id.time_1);

        auto = (Button)findViewById(R.id.auto_b);
        determine = (Button)findViewById(R.id.determine_1);


        //m_Going = (Button)findViewById(R.id.going_bt);
        //m_Backing = (Button)findViewById(R.id.backing_bt);

        m_goForwardButton.setOnClickListener(new ButtonListener());
        m_turnLeftButton.setOnClickListener(new ButtonListener());
        m_StopButton.setOnClickListener(new ButtonListener());
        m_turnRightButton.setOnClickListener(new ButtonListener());
        m_turnBackButton.setOnClickListener(new ButtonListener());
        //m_Going.setOnClickListener(new ButtonListener());
        //m_Backing.setOnClickListener(new ButtonListener());
        m_cat1.setOnClickListener(new ButtonListener());
        m_cat2.setOnClickListener(new ButtonListener());
        m_cat3.setOnClickListener(new ButtonListener());
        m_cat4.setOnClickListener(new ButtonListener());
        m_dog1.setOnClickListener(new ButtonListener());
        m_dog2.setOnClickListener(new ButtonListener());
        m_dog3.setOnClickListener(new ButtonListener());
        petb.setOnClickListener(new ButtonListener());
        m_dog4.setOnClickListener(new ButtonListener());
        auto.setOnClickListener(new ButtonListener());
        determine.setOnClickListener(new ButtonListener());



        m_goForwardButton.setOnLongClickListener(new ButtonListener());
        m_turnBackButton.setOnLongClickListener(new ButtonListener());
        m_turnLeftButton.setOnLongClickListener(new ButtonListener());
        m_turnRightButton.setOnLongClickListener(new ButtonListener());
    }



    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                this.finish();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private class ButtonListener implements View.OnClickListener, View.OnLongClickListener {

        @SuppressLint("WrongConstant")
        @Override
        public void onClick(View view) {
            int code = -2;
            switch (view.getId()){
                case R.id.cat_bt1:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"catfood","50");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "cat food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.cat_bt2:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"catfood","80");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "cat food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;


                case R.id.cat_bt3:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"catfood","30");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "cat food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.cat_bt4:
                    String inputText2=edittext2.getText().toString();
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"catfood",inputText2);
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "cat food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.dog_bt1:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"dogfood","50");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "confirm", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.dog_bt2:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"dogfood","80");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "dog food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.dog_bt3:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"dogfood","30");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "dog food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.pet_b:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","petb");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "pet button", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.dog_bt4:
                    String inputText1=edittext1.getText().toString();
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"dogfood",inputText1);
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "dog food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.determine_1:
                    String inputText3=time1.getText().toString();
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl",inputText3);
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "dog food", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.auto_b:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","autob");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "auto fellow", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;

                case R.id.go_forward_bt: //前进
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","forward");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "go forward", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_left_bt:  //向左
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","left");
                    if(code == 0){;
                        Toast.makeText(GosCarControlActivity.this, "turn_left", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.stop_bt:       //暂停
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","stop");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "stop", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_right_bt: //向右
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","right");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "turn_right", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_back_bt:  //向后
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","back");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "turn_back", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                default:
                    break;
            }
        }

        @Override
        public boolean onLongClick(View view) {
            int code = -2;
            switch (view.getId()){
                case R.id.go_forward_bt:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","forward");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "go forward", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_back_bt:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","backing");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "go forward", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_left_bt:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","lefting");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "turn_left_bt", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.turn_right_bt:
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"CarControl","righting");
                    if(code == 0){
                        Toast.makeText(GosCarControlActivity.this, "turn_right_bt", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosCarControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                default:
                    break;
            }
            return false;
        }
    }
}
