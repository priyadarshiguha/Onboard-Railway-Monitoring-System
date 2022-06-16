import mysql.connector
import serial


mydb = mysql.connector.connect(
  host="localhost",
  user="root",
  password="",
  database="hsr"
)

mycursor = mydb.cursor()
ser = serial.Serial('COM1', 9600);
train_no = 12345

while 1:
	data = ser.readline()
	
	if len(data) == 6 :
		print('RFID Location Recieved')
		data = str(data)[2:-5]
		print(data)
		sql = "INSERT INTO rfid_location(train_no, tag_id, latitude, longitude, timestamp) values (%s, %s, %s, %s, NOW())"
		val = (str(train_no), str(45677), str(30.34044), str(-97.81991))
		mycursor.execute(sql, val)
		mydb.commit()
		print(mycursor.rowcount, "record inserted into rfid_location.")

	elif len(data) > 6:
		print('Accident Alert Generated')
		data = str(data)[2:-5]
		print(data)
		sql = "INSERT INTO alerts(train_no, status, latitude, longitude, timestamp) values (%s, %s, %s, %s, NOW())"
		val = (str(train_no), data.split(';')[0], data.split(';')[1], data.split(';')[2])
		mycursor.execute(sql, val)
		mydb.commit()
		print(mycursor.rowcount, "record inserted into alerts.")
	else:
		pass