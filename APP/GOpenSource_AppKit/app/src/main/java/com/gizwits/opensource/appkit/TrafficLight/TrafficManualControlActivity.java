package com.gizwits.opensource.appkit.TrafficLight;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class TrafficManualControlActivity extends GosBaseActivity {
    private Button traffic_manual_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String traffic_manual_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_traffic_manual_control);
        initDevice();
        traffic_manual_led_control_init();
        setActionBar(true, true, "Traffic_Manual_Control_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(traffic_manual_led_status.equals("off")) {
            status = "red_on";
        }
        if(traffic_manual_led_status.equals("red_on")) {
            status = "yellow_on";
        }
        if(traffic_manual_led_status.equals("yellow_on")) {
            status = "green_on";
        }
        if(traffic_manual_led_status.equals("green_on")) {
            status = "red_on";
        }
        return  status;
    }

    private  void setText() {
        if (traffic_manual_led_status.equals("red_on")){
            traffic_manual_led_Button.setText(R.string.Traffic_reg);
            traffic_manual_led_Button.setBackgroundResource(R.drawable.bt_red);
        }
        if (traffic_manual_led_status.equals("yellow_on")){
            traffic_manual_led_Button.setText(R.string.Traffic_yellow);
            traffic_manual_led_Button.setBackgroundResource(R.drawable.bt_yellow);
        }
        if (traffic_manual_led_status.equals("green_on")){
            traffic_manual_led_Button.setText(R.string.Traffic_green);
            traffic_manual_led_Button.setBackgroundResource(R.drawable.bt_green);
        }
    }
    /* traffic_manual_control_Module*/
    private void traffic_manual_led_control_init(){
        traffic_manual_led_Button = (Button) findViewById(R.id.Traffic_reg_bt);
        traffic_manual_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "tl_s", status);
                if (code == 0) {
                    traffic_manual_led_status = status;
                    setText();
                } else if (code == -2) {
                }
            }
        });
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
}