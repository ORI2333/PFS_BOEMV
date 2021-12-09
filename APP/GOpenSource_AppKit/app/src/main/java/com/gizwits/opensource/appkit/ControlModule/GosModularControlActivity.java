package com.gizwits.opensource.appkit.ControlModule;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class GosModularControlActivity extends GosBaseActivity {

    private Button m_engine_left_bt;
    private Button m_engine_right_bt;
    private Button m_engine_middle_bt;
    private Button m_trace_module_bt;
    private Button m_ultrasonic_module_bt;
    private Button m_steer_engine_bt;

    /** The GizWifiDevice device */
    private GizWifiDevice device;
    /** The ActionBar actionBar */
    ActionBar actionBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gos_modular_control);
        initDevice();
        setActionBar(true, true, "ModularControl");
        initView();
    }

    private void initView() {
        m_engine_left_bt = (Button)findViewById(R.id.engine_left_bt);
        m_engine_right_bt = (Button)findViewById(R.id.engine_right_bt);
        m_engine_middle_bt = (Button)findViewById(R.id.engine_middle_bt);
        m_trace_module_bt = (Button)findViewById(R.id.trace_module_bt);
        m_ultrasonic_module_bt = (Button)findViewById(R.id.ultrasonic_module_bt);
        m_steer_engine_bt = (Button)findViewById(R.id.steer_engine_bt);

        m_engine_left_bt.setOnClickListener(new ButtonListener());
        m_engine_right_bt.setOnClickListener(new ButtonListener());
        m_engine_middle_bt.setOnClickListener(new ButtonListener());
        m_trace_module_bt.setOnClickListener(new ButtonListener());
        m_ultrasonic_module_bt.setOnClickListener(new ButtonListener());
        m_steer_engine_bt.setOnClickListener(new ButtonListener());
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

    private class ButtonListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            int code = -2;
            switch(view.getId()){
                case R.id.engine_left_bt://舵机向左
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","engine_left");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "engine_left", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.engine_right_bt://舵机向右
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","engine_right");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "engine_right", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.engine_middle_bt://舵机归中
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","engine_middle");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "engine_middle", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.trace_module_bt://启动循迹模块
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","trace_module");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "trace_module", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.ultrasonic_module_bt://启动超声波模块
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","ultrasonic_module");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "ultrasonic_module", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                case R.id.steer_engine_bt://启动舵机超声波模块
                    code = GizWifiSDK.sharedInstance().post(device.m_index,"ModularControl","steer_engine");
                    if(code == 0){
                        Toast.makeText(GosModularControlActivity.this, "steer_engine", 100).show();//add by wgm
                    }else if(code == -2){
                        Toast.makeText(GosModularControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}