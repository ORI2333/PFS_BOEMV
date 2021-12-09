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

public class LightDetectActivity extends GosBaseActivity {
    private Button light_detect_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String light_detect_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_light_detect);
        initDevice();
        light_detect_led_control_init();
        setActionBar(true, true, "Light_Detect_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(light_detect_led_status.equals("off")) {
            status = "lt";
        }
        return  status;
    }

    private  void setText() {
        if (light_detect_led_status.equals("lt")){
            light_detect_led_Button.setText(R.string.light_detect_start);
            light_detect_led_Button.setBackgroundResource(R.drawable.cd_68);
        }
    }
    /* light_detect_control_Module*/
    private void light_detect_led_control_init(){
        light_detect_led_Button = (Button) findViewById(R.id.bt_light_detect);
        light_detect_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "lt_s", status);
                if (code == 0) {
                    light_detect_led_status = status;
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