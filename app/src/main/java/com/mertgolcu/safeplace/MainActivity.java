package com.mertgolcu.safeplace;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {


    TextView txt_date, txt_now_enter, txt_enter, txt_exit, txt_total_enter, txt_max_enter;
    ProgressBar pb;
    FirebaseDatabase database = FirebaseDatabase.getInstance();
    DatabaseReference myRef = database.getReference();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txt_date = findViewById(R.id.txt_date);
        txt_now_enter = findViewById(R.id.txt_now_enter);
        txt_enter = findViewById(R.id.txt_enter_count);
        txt_exit = findViewById(R.id.txt_exit_count);
        txt_total_enter = findViewById(R.id.txt_total_enter_count);
        txt_max_enter = findViewById(R.id.max_total_enter_count);
        pb = findViewById(R.id.main_pb);

        txt_date.setText(new SimpleDateFormat("dd-MM-yyyy").format(new Date()));

        pb.setVisibility(View.VISIBLE);
        myRef.child("live").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // This method is called once with the initial value and again
                // whenever data at this location is updated.
                long enter = (long) dataSnapshot.child("giris").getValue();
                long exit = (long) dataSnapshot.child("cikis").getValue();

                long now = enter - exit;

                txt_now_enter.setText(String.valueOf(now));
                txt_enter.setText(String.valueOf(enter));
                txt_exit.setText(String.valueOf(exit));
                txt_total_enter.setText(String.valueOf(enter));
                txt_max_enter.setText(String.valueOf(Constants.max_person));

                pb.setVisibility(View.GONE);

            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                Log.w("TAG", "Failed to read value.", error.toException());
            }
        });

    }
}