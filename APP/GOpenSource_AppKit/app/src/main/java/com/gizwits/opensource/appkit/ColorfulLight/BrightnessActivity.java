package com.gizwits.opensource.appkit.ColorfulLight;

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

public class BrightnessActivity extends GosBaseActivity {
    private Button brightness_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String brightness_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_brightness);
        initDevice();
        brightness_led_control_init();
        setActionBar(true, true, "Brightness_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(brightness_led_status.equals("off")) {
            status = "br_l";
        }
        if(brightness_led_status.equals("br_l")){
            status = "br_m";
        }
        if(brightness_led_status.equals("br_m")){
            status = "br_h";
        }
        if(brightness_led_status.equals("br_h")){
            status = "br_l";
        }
        return  status;
    }

    private  void setText() {
        if (brightness_led_status.equals("br_l")){
            brightness_led_Button.setText(R.string.bright_low);
            brightness_led_Button.setBackgroundResource(R.drawable.track);
        } else if(brightness_led_status.equals("br_m")){
            brightness_led_Button.setText(R.string.bright_middle);
            brightness_led_Button.setBackgroundResource(R.drawable.solid);
        } else if (brightness_led_status.equals("br_h")) {
            brightness_led_Button.setText(R.string.bright_high);
            brightness_led_Button.setBackgroundResource(R.drawable.bt_white);
        }
    }
    /* brightness_control_Module*/
    private void brightness_led_control_init(){
        brightness_led_Button = (Button) findViewById(R.id.bt_brightness);
        brightness_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "br_s", status);
                if (code == 0) {
                    brightness_led_status = status;
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