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

public class TrafficHumanModuleActivity extends GosBaseActivity {
    private Button traffic_human_led_Button;
    private Button traffic_human_control_Button;
    Intent intent;
    private GizWifiDevice device;
    private String traffic_human_led_status = "off";
    private String traffic_human_control_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_traffic_human_module);
        initDevice();
        traffic_human_led_control_init();
        traffic_human_led_control_by_human();
        setActionBar(true, true, "Traffic_Human_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(traffic_human_led_status.equals("off")) {
            status = "tlh";
        }
        return  status;
    }
    private String switchStatusHuman (){
        String status = "off";
        if(traffic_human_control_status.equals("off")) {
            status = "tlm";
        }
        return  status;
    }
    private  void setText() {
        if (traffic_human_led_status.equals("tlh")){
            traffic_human_led_Button.setText(R.string.Traffic_Human_Module_Start);
            traffic_human_led_Button.setBackgroundResource(R.drawable.solid);
        }
    }
    private  void setTextHuman() {
        if (traffic_human_control_status.equals("tlm")){
            traffic_human_control_Button.setText(R.string.Traffic_Human_Module_control);
            traffic_human_control_Button.setBackgroundResource(R.drawable.flesh);
        }
    }
    /* traffic_human_control_Module normal*/
    private void traffic_human_led_control_init(){
        traffic_human_led_Button = (Button) findViewById(R.id.bt_traffic_human_normal);
        traffic_human_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "tlh_s", status);
                if (code == 0) {
                    traffic_human_led_status = status;
                    setText();
                } else if (code == -2) {
                }
            }
        });
    }

    private void traffic_human_led_control_by_human(){
        traffic_human_control_Button = (Button) findViewById(R.id.bt_traffic_human_control);
        traffic_human_control_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatusHuman();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "tlm_s", status);
                if (code == 0) {
                    traffic_human_control_status = status;
                    setTextHuman();
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