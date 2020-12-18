package com.mertgolcu.safeplace;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class LoginActivity extends AppCompatActivity {

    private Button login_button;
    private EditText edt_employee, edt_place_size;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        login_button = findViewById(R.id.btn_login);
        edt_employee = findViewById(R.id.edt_employee_count);
        edt_place_size = findViewById(R.id.edt_place_size);
        login_button.setVisibility(View.GONE);
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        DatabaseReference myRef = database.getReference();


        myRef.child("place").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // This method is called once with the initial value and again
                // whenever data at this location is updated.
                long emp_count;
                long max_person;
                long size;


                emp_count = (long) dataSnapshot.child("calisan_sayisi").getValue();
                max_person = (long) dataSnapshot.child("max_kisi").getValue();
                size = (long) dataSnapshot.child("metrekare").getValue();

                if (emp_count == 0 && max_person == 0 && size == 0) {

                    login_button.setVisibility(View.VISIBLE);
                } else {
                    Constants.max_person = max_person;
                    go_main();
                }
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                Log.w("TAG", "Failed to read value.", error.toException());
            }
        });
        login_button.setOnClickListener(v -> {
            long emp_count = Long.parseLong(edt_employee.getText().toString());
            long size = Long.parseLong(edt_place_size.getText().toString());
            long max_person = (size / 4) - emp_count;
            myRef.child("place").child("calisan_sayisi").setValue(emp_count);
            myRef.child("place").child("max_kisi").setValue(max_person);
            myRef.child("place").child("metrekare").setValue(size);
            Constants.max_person = max_person;
            go_main();
        });
    }

    private void go_main() {
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivity(intent);
        finish();
    }
}