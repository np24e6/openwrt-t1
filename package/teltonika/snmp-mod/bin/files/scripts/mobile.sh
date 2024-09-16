
mdcollect=$1

[ "$mdcollect" -eq "1" ] && {
    . "$MODULES_DIR/mdcollect"
}

cat << EOF
-- GSM --

modemGroup OBJECT-GROUP
	OBJECTS { modemNum,
		  mIndex,
		  mDescr,
		  mImei,
		  mModel,
		  mManufacturer,
		  mRevision,
		  mSerial,
		  mIMSI,
		  mSimState,
		  mPinState,
		  mNetState,
		  mSignal,
		  mOperator,
		  mOperatorNumber,
		  mConnectionState,
		  mNetworkType,
		  mTemperature,
		  mCellID,
		  mSINR,
		  mRSRP,
		  mRSRQ,
		  $mdcollect_group
		  mIP,
		  $mdcollect_group1
		  mICCID,
		  $mdcollect_group2
		  $mdcollect_group3
		  connectionUptime }
	STATUS current
	DESCRIPTION "Mobile SNMP group defined according to RFC 2580"
	::= { teltonikaSnmpGroups 2 }

modemNum OBJECT-TYPE
	SYNTAX		Integer32
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"The number of modems (regardless of their current state) present on this system"
	::= { mobile 1 }

-- The modems table

modemTable OBJECT-TYPE
	SYNTAX		SEQUENCE OF ModemEntry
	MAX-ACCESS	not-accessible
	STATUS		current
	DESCRIPTION	"A list of modem entries. The number of entries is given by the value of modemNum"
	::= { mobile 2 }

modemEntry OBJECT-TYPE
	SYNTAX		ModemEntry
	MAX-ACCESS	not-accessible
	STATUS		current
	DESCRIPTION	"An entry containing information of a particular interface"
	INDEX		{ mIndex }
	::= { modemTable 1 }

ModemEntry ::=
	SEQUENCE {
	mIndex			Integer32,
	mDescr			DisplayString,
	mImei			DisplayString,
	mModel			DisplayString,
	mManufacturer		DisplayString,
	mRevision		DisplayString,
	mSerial 		DisplayString,
	mIMSI			DisplayString,
	mSimState		DisplayString,
	mPinState		DisplayString,
	mNetState		DisplayString,
	mSignal		INTEGER,
	mOperator		DisplayString,
	mOperatorNumber 	DisplayString,
	mConnectionState	DisplayString,
	mNetworkType		DisplayString,
	mTemperature		INTEGER,
	mCellID		DisplayString,
	mSINR			DisplayString,
	mRSRP			DisplayString,
	mRSRQ			DisplayString,
	$mdcollect_entry
	mIP			DisplayString,
	$mdcollect_entry1
	$mdcollect_entry2
	$mdcollect_entry3
	mICCID			DisplayString
	}

mIndex OBJECT-TYPE
	SYNTAX		Integer32 (1..2147483647)
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"A unique value, greater than zero, for each modem"
	::= { modemEntry 1 }

mDescr OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"A textual string containing information about the modem"
	::= { modemEntry 2 }

mImei OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem IMEI"
	::= { modemEntry 3 }

mModel OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem model"
	::= { modemEntry 4 }

mManufacturer OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem manufacturer"
	::= { modemEntry 5 }

mRevision OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem revision"
	::= { modemEntry 6 }

mSerial OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Serial number"
	::= { modemEntry 7 }

mIMSI OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"IMSI"
	::= { modemEntry 8 }

mSimState OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"SIM status"
	::= { modemEntry 9 }

mPinState OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"PIN status"
	::= { modemEntry 10 }

mNetState OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Mobile network registration status"
	::= { modemEntry 11 }

mSignal OBJECT-TYPE
	SYNTAX		INTEGER
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Signal level"
	::= { modemEntry 12 }

mOperator OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Operator currently in use"
	::= { modemEntry 13 }

mOperatorNumber OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Operator number (MCC+MNC)"
	::= { modemEntry 14 }

mConnectionState OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Data session connection state"
	::= { modemEntry 15 }

mNetworkType OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Data session network type"
	::= { modemEntry 16 }

mTemperature OBJECT-TYPE
	SYNTAX		INTEGER
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem temperature"
	::= { modemEntry 17 }

mCellID OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"CELL ID"
	::= { modemEntry 18 }

mSINR OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"LTE SINR level"
	::= { modemEntry 19 }

mRSRP OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"LTE RSRP level"
	::= { modemEntry 20 }

mRSRQ OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"LTE RSRQ level"
	::= { modemEntry 21 }

$mdcollect_obj

mIP OBJECT-TYPE
	SYNTAX		DisplayString (SIZE (0..255))
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Modem IP address(es)"
	::= { modemEntry 24 }

$mdcollect_obj1

mICCID OBJECT-TYPE
	SYNTAX      DisplayString (SIZE (0..255))
	MAX-ACCESS  read-only
	STATUS      current
	DESCRIPTION "SIM ICCID"
	::= { modemEntry 27 }

$mdcollect_obj2

$mdcollect_obj3

-- End of the table

connectionUptime OBJECT-TYPE
	SYNTAX		Unsigned32
	MAX-ACCESS	read-only
	STATUS		current
	DESCRIPTION	"Mobile connection uptime in seconds"
	::= { mobile 3 }
EOF
