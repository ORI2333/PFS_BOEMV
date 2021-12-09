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

public class TrafficAutoModuleActivity extends GosBaseActivity {
    private Button traffic_auto_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String traffic_auto_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_traffic_auto_module);
        initDevice();
        traffic_auto_led_control_init();
        setActionBar(true, true, "Traffic_Auto_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(traffic_auto_led_status.equals("off")) {
            status = "tla";
        }
        return  status;
    }

    private  void setText() {
        if (traffic_auto_led_status.equals("tla")){
            traffic_auto_led_Button.setText(R.string.Traffic_Auto_Module_Start);
            traffic_auto_led_Button.setBackgroundResource(R.drawable.bt_red);
        }
    }
    /* traffic_auto_control_Module*/
    private void traffic_auto_led_control_init(){
        traffic_auto_led_Button = (Button) findViewById(R.id.bt_traffic_auto);
        traffic_auto_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "tla_s", status);
                if (code == 0) {
                    traffic_auto_led_status = status;
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