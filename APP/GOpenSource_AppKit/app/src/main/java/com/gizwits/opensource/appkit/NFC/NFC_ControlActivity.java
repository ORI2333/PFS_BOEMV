package com.gizwits.opensource.appkit.NFC;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentGasActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentHumidityActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentTemperatureActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentValueActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;

public class NFC_ControlActivity extends GosBaseActivity {

    private Button m_nfc_wechat_Control_Module_Button;
    private Button m_nfc_today_headline_Module_Button;
    private Button m_taobao_Module_Button;
    private Button m_huawei_smart_lift_Module_Button;
    private Button m_histreaming_Module_Button;
    private Button m_return_Module_Button;
    Intent intent;
    private GizWifiDevice device;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_f_c__control);
        initDevice();
        environment_view_init();
        setActionBar(true, true, "Environment");
    }
    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    private void environment_view_init() {
        m_nfc_wechat_Control_Module_Button = (Button)findViewById(R.id.bt_nfc_wechat_app);
        m_nfc_today_headline_Module_Button = (Button)findViewById(R.id.bt_nfc_today_headline_app);
        m_taobao_Module_Button =(Button)findViewById(R.id.bt_nfc_taobao_app);
        m_huawei_smart_lift_Module_Button =(Button)findViewById(R.id.bt_nfc_huawei_smart_lift_app);
        m_histreaming_Module_Button =(Button)findViewById(R.id.bt_nfc_histreaming_app);
        m_return_Module_Button =(Button)findViewById(R.id.bt_nfc_return);
        /*wechat app */
        m_nfc_wechat_Control_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_WeChatActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*today headline app*/
        m_nfc_today_headline_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_TodayHeadLineActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*taobao app*/
        m_taobao_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_TaobaoActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*huawei smart lift*/
        m_huawei_smart_lift_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_HuaweiSmartLiftActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*histreaming*/
        m_histreaming_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_HistreamingActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*return main menu*/
        m_return_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(NFC_ControlActivity.this, NFC_ReturnActivity.class);
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