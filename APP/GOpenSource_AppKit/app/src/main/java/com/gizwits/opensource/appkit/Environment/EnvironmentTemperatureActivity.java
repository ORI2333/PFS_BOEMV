package com.gizwits.opensource.appkit.Environment;

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

public class EnvironmentTemperatureActivity extends GosBaseActivity {
    private Button environment_temperature_control_Button;
    Intent intent;
    private GizWifiDevice device;
    private String environment_temperature_control_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_environment_temperature);
        initDevice();
        environment_temperature_control_init();
        setActionBar(true, true, "Environment");
    }
    private String switchStatus (){
        String status = "off";
        if(environment_temperature_control_status.equals("off")) {
            status = "ent";
        }
        return  status;
    }

    private  void setText() {
        if (environment_temperature_control_status.equals("ent")) {
            environment_temperature_control_Button.setText(R.string.environment_temperature);
            environment_temperature_control_Button.setBackgroundResource(R.drawable.bt_red);
        }
    }
    /* environment_temperature_control_Module*/
    private void environment_temperature_control_init(){
        environment_temperature_control_Button = (Button) findViewById(R.id.bt_environment_temperature);
        environment_temperature_control_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "ent_s", status);
                if (code == 0) {
                    environment_temperature_control_status = status;
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