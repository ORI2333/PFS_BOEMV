package com.gizwits.opensource.appkit.Environment;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.gizwits.opensource.appkit.TrafficLight.TrafficAutoModuleActivity;
import com.gizwits.opensource.appkit.TrafficLight.TrafficHumanModuleActivity;
import com.gizwits.opensource.appkit.TrafficLight.TrafficLightActivity;
import com.gizwits.opensource.appkit.TrafficLight.TrafficManualControlActivity;
import com.hisilicion.histreaming.GizWifiDevice;

public class EnvironmentActivity extends GosBaseActivity {
    private Button m_environment_all_Control_Module_Button;
    private Button m_environment_temperature_Module_Button;
    private Button m_environment_humidity_Module_Button;
    private Button m_environment_gas_Module_Button;
    private Button m_environment_return_Button;
    Intent intent;
    private GizWifiDevice device;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_environment);
        initDevice();
        environment_view_init();
        setActionBar(true, true, "Environment");
    }
    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    private void environment_view_init() {
        m_environment_all_Control_Module_Button = (Button)findViewById(R.id.bt_environment_all_value);
        m_environment_temperature_Module_Button = (Button)findViewById(R.id.bt_environment_temperature_value);
        m_environment_humidity_Module_Button =(Button)findViewById(R.id.bt_environment_humidity_value);
        m_environment_gas_Module_Button =(Button)findViewById(R.id.bt_environment_gas_value);
        m_environment_return_Button =(Button)findViewById(R.id.bt_environment_return_main_menu);
        /*environment all */
        m_environment_all_Control_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(EnvironmentActivity.this, EnvironmentValueActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*environment temperature*/
        m_environment_temperature_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(EnvironmentActivity.this, EnvironmentTemperatureActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*environment humidity*/
        m_environment_humidity_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(EnvironmentActivity.this, EnvironmentHumidityActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*environment gas*/
        m_environment_gas_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(EnvironmentActivity.this, EnvironmentGasActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*environment return main menu*/
        m_environment_return_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(EnvironmentActivity.this, EnvironmentReturnActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
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