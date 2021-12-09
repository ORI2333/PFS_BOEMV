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

public class EnvironmentReturnActivity extends GosBaseActivity {
    private Button return_main_menu_Button;
    Intent intent;
    private GizWifiDevice device;
    private String return_main_menu_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_environment_return);
        initDevice();
        environment_return_main_menu();
        setActionBar(true, true, "Return_Menu_Module");
    }
    private String switchStatus (){
        String status = "off";
        if (return_main_menu_status.equals("off")) {
            status = "enr";
        }
        return  status;
    }

    private  void setText() {
        if (return_main_menu_status.equals("enr")) {
            return_main_menu_Button.setText(R.string.environment_return_main_menu);
            return_main_menu_Button.setBackgroundResource(R.drawable.cd_68);
        }
    }
    /* return_main_menu*/
    private void environment_return_main_menu(){
        return_main_menu_Button = (Button) findViewById(R.id.bt_environment_return_menu);
        return_main_menu_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "enr_s", status);
                if (code == 0) {
                    return_main_menu_status = status;
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