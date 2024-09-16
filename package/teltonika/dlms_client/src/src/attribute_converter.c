#include "master.h"

/*
	Original: https://github.com/Gurux/GuruxDLMS.c/blob/master/development/src/converters.c
	Rewritten to match JSON format.
*/

PRIVATE void parse_scaler(char **json, char *value);

PUBLIC void attr_init(object_attributes *attributes, gxObject *object)
{
	attributes->values = calloc(obj_attributeCount(object), sizeof(char *));
	if (!attributes->values) {
		log(L_ERROR, "Failed to initiate COSEM attribute values");
		exit(1);
	}
}

PUBLIC void attr_free(object_attributes *attributes)
{
	if (!attributes) {
		return;
	}

	for (int i = 0; i < attributes->count; i++) {
		free(attributes->values[i]);
	}

	free(attributes->values);
	attributes->count = 0;
}

PUBLIC char *attr_to_json(object_attributes *attributes)
{
	char *json = NULL;
	for (int i = 0; i < attributes->count && attributes->values[i]; i++) {
		char attr_no[16] = { 0 };
		snprintf(attr_no, sizeof(attr_no), "\"%d\": ", i + 1);
		utl_append_to_str(&json, attr_no);
		if (strstr(attributes->values[i], "Scaler") || strstr(attributes->values[i], "Cell ID")) {
			parse_scaler(&json, attributes->values[i]);
			utl_append_if_needed(&json, i, attributes->count - 1, ",");
			continue;
		}

		utl_append_to_str(&json, "\"");
		utl_append_to_str(&json, attributes->values[i]);
		utl_append_to_str(&json, "\"");
		utl_append_if_needed(&json, i, attributes->count - 1, ", ");
	}

	return json;
}

PRIVATE void parse_scaler(char **json, char *value)
{
	utl_append_to_str(json, "{");
	utl_append_to_str(json, value);
	utl_append_to_str(json, "}");
}

PRIVATE int obj_timeWindowToString(gxArray* arr, gxByteBuffer* bb)
{
    gxKey* it;
    int ret = 0;
    uint16_t pos;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
        {
            break;
        }
        if (pos != 0)
        {
            bb_addString(bb, ", ");
        }
        if ((ret = time_toString((gxtime*)it->key, bb)) != 0 ||
            (ret = bb_addString(bb, " ")) != 0 ||
            (ret = time_toString((gxtime*)it->value, bb)) != 0)
        {
            break;
        }
    }
    return ret;
}

PRIVATE int obj_CaptureObjectsToString(gxByteBuffer* ba, gxArray* objects)
{
    uint16_t pos;
    int ret = DLMS_ERROR_CODE_OK;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxKey* it;
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    for (pos = 0; pos != objects->size; ++pos)
    {
        if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->target->objectType))) != 0 ||
            (ret = bb_addString(ba, " ")) != 0 ||
            (ret = hlp_appendLogicalName(ba, it->target->logicalName)) != 0)
        {
            break;
        }
#else
        if ((ret = bb_addString(ba, obj_typeToString2(((gxObject*)it->key)->objectType))) != 0 ||
            (ret = bb_addString(ba, " ")) != 0 ||
            (ret = hlp_appendLogicalName(ba, ((gxObject*)it->key)->logicalName)) != 0)
        {
            break;
        }
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    }
    return ret;
}

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
PRIVATE int obj_seasonProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxSeasonProfile* it;
    int ret;
    uint16_t pos;
    bb_addString(ba, "[");
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = arr_getByIndex(arr, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_addString(ba, "{");
        bb_attachString(ba, bb_toHexString(&it->name));
        bb_addString(ba, ", ");
        time_toString(&it->start, ba);
        bb_addString(ba, ", ");
        bb_attachString(ba, bb_toHexString(&it->weekName));
        bb_addString(ba, "}");
    }
    bb_addString(ba, "]");
    return 0;
}

PRIVATE int obj_weekProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxWeekProfile* it;
    int ret;
    uint16_t pos;
    bb_addString(ba, "[");
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = arr_getByIndex(arr, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_attachString(ba, bb_toString(&it->name));
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->monday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->tuesday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->wednesday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->thursday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->friday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->saturday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->sunday);
    }
    bb_addString(ba, "]");
    return 0;
}

PRIVATE int obj_dayProfileToString(gxArray *arr, gxByteBuffer *ba)
{
	gxDayProfile *dp = NULL;
	gxDayProfileAction *it = NULL;
	int ret;
	uint16_t pos, pos2;
	bb_addString(ba, "[");

	for (pos = 0; pos != arr->size; ++pos) {
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
		ret = arr_getByIndex(arr, pos, (void **)&dp);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		bb_addIntAsString(ba, dp->dayId);
		bb_addString(ba, "[");
		for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2) {
			if (pos2 != 0) {
				bb_addString(ba, ", ");
			}

			ret = arr_getByIndex(&dp->daySchedules, pos2, (void **)&it);
			if (ret != DLMS_ERROR_CODE_OK) {
				return ret;
			}

#ifndef DLMS_IGNORE_OBJECT_POINTERS
		if (!it || !it->script) {
			hlp_appendLogicalName(ba, EMPTY_LN);
		} else {
			hlp_appendLogicalName(ba, it->script->logicalName);
		}
#else
	    	hlp_appendLogicalName(ba, it->scriptLogicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->scriptSelector);
		bb_addString(ba, " ");
		time_toString(&it->startTime, ba);
		}
	bb_addString(ba, "]");
    }
    bb_addString(ba, "]");
    return 0;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

PRIVATE int obj_objectsToString(gxByteBuffer* ba, objectArray* objects)
{
    uint16_t pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* it;
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = oa_getByIndex(objects, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->objectType));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(ret == 0);
#endif
        bb_addString(ba, " ");
        hlp_appendLogicalName(ba, it->logicalName);
        }
    return ret;
}

PRIVATE int obj_getIPAddress(gxByteBuffer* ba, gxArray* arr)
{
    char tmp[64];
    int ret;
    uint16_t pos;
    IN6_ADDR* ip;
    if ((ret = bb_addString(ba, "{")) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            if ((ret = arr_getByIndex(arr, pos, (void**)&ip)) != 0)
            {
                break;
            }
            if (pos != 0)
            {
                bb_addString(ba, ", ");
            }
            //Add Ws2_32.lib for LabWindows/CVI.
            inet_ntop(AF_INET6, &ip, tmp, sizeof(tmp));
            bb_addString(ba, tmp);
        }
        if (ret == 0)
        {
            ret = bb_addString(ba, "}");
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_REGISTER_MONITOR
PRIVATE void actionItemToString(gxActionItem* item, gxByteBuffer* ba)
{
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (item->script == NULL)
    {
        hlp_appendLogicalName(ba, EMPTY_LN);
    }
    else
    {
        hlp_appendLogicalName(ba, item->script->base.logicalName);
    }
#else
    hlp_appendLogicalName(ba, item->logicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(ba, " ");
    bb_addIntAsString(ba, item->scriptSelector);
}

#endif

PRIVATE int obj_getNeighborDiscoverySetupAsString(gxByteBuffer* ba, gxArray* arr)
{
    int ret;
    uint16_t pos;
    gxNeighborDiscoverySetup* it;
    if ((ret = bb_addString(ba, "{")) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
            {
                break;
            }
            if (pos != 0)
            {
                bb_addString(ba, ", ");
            }
            if ((ret = bb_addString(ba, "[")) != 0 ||
                (ret = bb_addIntAsString(ba, it->maxRetry)) != 0 ||
                (ret = bb_addString(ba, ", ")) != 0 ||
                (ret = bb_addIntAsString(ba, it->retryWaitTime)) != 0 ||
                (ret = bb_addString(ba, ", ")) != 0 ||
                (ret = bb_addIntAsString(ba, it->sendPeriod)) != 0 ||
                (ret = bb_addString(ba, "]")) != 0)
            {
                break;
            }
        }
        if (ret == 0)
        {
            ret = bb_addString(ba, "}");
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA
PUBLIC int attr_DataToString(gxData *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	ret					= var_toString(&object->value, &ba);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_DATA

#ifndef DLMS_IGNORE_REGISTER
PUBLIC int attr_RegisterToString(gxRegister *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	ret					= var_toString(&object->value, &ba);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	// TODO: probably needs better formatting
	bb_addString(&ba, "\"Scaler\": \"");
	bb_addDoubleAsString(&ba, hlp_getScaler(object->scaler));
	bb_addString(&ba, "\", \"Unit\": \"");
	bb_addString(&ba, obj_getUnitAsString(object->unit));
	bb_addString(&ba, "\"");

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
PUBLIC int attr_clockToString(gxClock *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	ret = time_toString(&object->time, &ba);
	if (ret != 0) {
		return ret;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->timeZone);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->status);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = time_toString(&object->begin, &ba);
	if (ret != 0) {
		return ret;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = time_toString(&object->end, &ba);
	if (ret != 0) {
		return ret;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->deviation);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->enabled);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->clockBase);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
PUBLIC int attr_ScriptTableToString(gxScriptTable *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos, pos2;
	gxByteBuffer ba;
	gxScript *s;
	gxScriptAction *sa;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->scripts.size; ++pos) {
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		ret = arr_getByIndex(&object->scripts, pos, (void **)&s);
		if (ret != 0) {
			return ret;
		}
		bb_addIntAsString(&ba, s->id);
		// bb_addString(&ba, "\n");
		for (pos2 = 0; pos2 != s->actions.size; ++pos2) {
			ret = arr_getByIndex(&s->actions, pos2, (void **)&sa);
			if (ret != 0) {
				return ret;
			}
			if (pos2 != 0) {
				bb_addString(&ba, ", ");
			}
			bb_addIntAsString(&ba, sa->type);
			bb_addString(&ba, " ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
			if (sa->target == NULL) {
				bb_addIntAsString(&ba, 0);
				bb_addString(&ba, " ");
				hlp_appendLogicalName(&ba, EMPTY_LN);
			} else {
				bb_addIntAsString(&ba, sa->target->objectType);
				bb_addString(&ba, " ");
				hlp_appendLogicalName(&ba, sa->target->logicalName);
			}
#else
			bb_addIntAsString(&ba, sa->objectType);
			bb_addString(&ba, " ");
			hlp_appendLogicalName(&ba, sa->logicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS

			bb_addString(&ba, " ");
			bb_addIntAsString(&ba, sa->index);
			bb_addString(&ba, " ");
			ret = var_toString(&sa->parameter, &ba);
			if (ret != 0) {
				return ret;
			}
		}
	}
	// bb_addString(&ba, "]\n");
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
PUBLIC int attr_specialDaysTableToString(gxSpecialDaysTable *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos;
	gxSpecialDay *sd;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->entries.size; ++pos) {
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		ret = arr_getByIndex(&object->entries, pos, (void **)&sd);
		if (ret != 0) {
			return ret;
		}
		bb_addIntAsString(&ba, sd->index);
		bb_addString(&ba, " ");
		ret = time_toString(&sd->date, &ba);
		if (ret != 0) {
			return ret;
		}
		bb_addString(&ba, " ");
		bb_addIntAsString(&ba, sd->dayId);
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
PUBLIC int attr_TcpUdpSetupToString(gxTcpUdpSetup *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->port);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

#ifndef DLMS_IGNORE_OBJECT_POINTERS
	bb_addLogicalName(&ba, obj_getLogicalName((gxObject *)object->ipSetup));
#else
	bb_addLogicalName(&ba, object->ipReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->maximumSegmentSize);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->maximumSimultaneousConnections);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->inactivityTimeout);
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP

PUBLIC int attr_timeWindowToString(gxArray *arr, gxByteBuffer *bb)
{
	gxKey *it;
	int ret = 0;
	uint16_t pos;
	for (pos = 0; pos != arr->size; ++pos) {
		if ((ret = arr_getByIndex(arr, pos, (void **)&it)) != 0) {
			break;
		}
		if (pos != 0) {
			bb_addString(bb, ", ");
		}
		if ((ret = time_toString((gxtime *)it->key, bb)) != 0 || (ret = bb_addString(bb, " ")) != 0 ||
		    (ret = time_toString((gxtime *)it->value, bb)) != 0) {
			break;
		}
	}
	return ret;
}

PUBLIC int attr_CaptureObjectsToString(gxByteBuffer *ba, gxArray *objects)
{
	uint16_t pos;
	int ret = DLMS_ERROR_CODE_OK;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	gxTarget *it;
#else
	gxKey *it;
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	for (pos = 0; pos != objects->size; ++pos) {
		if ((ret = arr_getByIndex(objects, pos, (void **)&it)) != DLMS_ERROR_CODE_OK) {
			break;
		}
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
		if ((ret = bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->target->objectType))) !=
			    0 ||
		    (ret = bb_addString(ba, " ")) != 0 ||
		    (ret = hlp_appendLogicalName(ba, it->target->logicalName)) != 0) {
			break;
		}
#else
		if ((ret = bb_addString(ba, obj_typeToString2(((gxObject *)it->key)->objectType))) != 0 ||
		    (ret = bb_addString(ba, " ")) != 0 ||
		    (ret = hlp_appendLogicalName(ba, ((gxObject *)it->key)->logicalName)) != 0) {
			break;
		}
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	}
	return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
PUBLIC int attr_pushSetupToString(gxPushSetup *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;

	if ((ret = BYTE_BUFFER_INIT(&ba)) != 0) {
		goto end;
	}

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if ((ret = obj_CaptureObjectsToString(&ba, &object->pushObjectList)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	// čia parsint reik list'ą.
	printf("%s\n", attributes->values[1]);
	bb_empty(&ba);

	if ((ret = bb_set(&ba, object->destination.data, object->destination.size)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, " ")) != 0) {
		goto end;
	}

	if ((ret = bb_addIntAsString(&ba, object->message)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_timeWindowToString(&object->communicationWindow, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->randomisationStartInterval)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->numberOfRetries)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->repetitionDelay)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
PUBLIC int attr_autoConnectToString(gxAutoConnect *object, object_attributes *attributes)
{
	gxKey *k;
	int ret;
	uint16_t pos;
	gxByteBuffer ba, *dest;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->mode);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->repetitions);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->repetitionDelay);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->callingWindow.size; ++pos) {
		// bb_addString(&ba, "\"");
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		ret = arr_getByIndex(&object->callingWindow, pos, (void **)&k);
		if (ret != 0) {
			return ret;
		}
		time_toString((gxtime *)k->key, &ba);
		bb_addString(&ba, " ");
		time_toString((gxtime *)k->value, &ba);
		// bb_addString(&ba, "\"");
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	for (pos = 0; pos != object->destinations.size; ++pos) {
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		ret = arr_getByIndex(&object->destinations, pos, (void **)&dest);
		if (ret != 0) {
			return ret;
		}
		bb_set2(&ba, dest, 0, bb_size(dest));
	}
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
PUBLIC int attr_seasonProfileToString(gxArray *arr, gxByteBuffer *ba)
{
	gxSeasonProfile *it;
	int ret;
	uint16_t pos;
	bb_addString(ba, "[");
	for (pos = 0; pos != arr->size; ++pos) {
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
		ret = arr_getByIndex(arr, pos, (void **)&it);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		bb_addString(ba, "{");
		bb_attachString(ba, bb_toHexString(&it->name));
		bb_addString(ba, ", ");
		time_toString(&it->start, ba);
		bb_addString(ba, ", ");
		bb_attachString(ba, bb_toHexString(&it->weekName));
		bb_addString(ba, "}");
	}
	bb_addString(ba, "]");
	return 0;
}
PUBLIC int attr_weekProfileToString(gxArray *arr, gxByteBuffer *ba)
{
	gxWeekProfile *it;
	int ret;
	uint16_t pos;
	bb_addString(ba, "[");
	for (pos = 0; pos != arr->size; ++pos) {
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
		ret = arr_getByIndex(arr, pos, (void **)&it);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		bb_attachString(ba, bb_toString(&it->name));
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->monday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->tuesday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->wednesday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->thursday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->friday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->saturday);
		bb_addString(ba, " ");
		bb_addIntAsString(ba, it->sunday);
	}
	bb_addString(ba, "]");
	return 0;
}

PUBLIC int attr_dayProfileToString(gxArray *arr, gxByteBuffer *ba)
{
	gxDayProfile *dp;
	gxDayProfileAction *it;
	int ret;
	uint16_t pos, pos2;
	bb_addString(ba, "[");
	for (pos = 0; pos != arr->size; ++pos) {
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
		ret = arr_getByIndex(arr, pos, (void **)&dp);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		bb_addIntAsString(ba, dp->dayId);
		bb_addString(ba, "[");
		for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2) {
			if (pos2 != 0) {
				bb_addString(ba, ", ");
			}
			ret = arr_getByIndex(&dp->daySchedules, pos2, (void **)&it);
			if (ret != DLMS_ERROR_CODE_OK) {
				return ret;
			}
#ifndef DLMS_IGNORE_OBJECT_POINTERS
			if (it->script == NULL) {
				hlp_appendLogicalName(ba, EMPTY_LN);
			} else {
				hlp_appendLogicalName(ba, it->script->logicalName);
			}
#else
			hlp_appendLogicalName(ba, it->scriptLogicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS
			bb_addString(ba, " ");
			bb_addIntAsString(ba, it->scriptSelector);
			bb_addString(ba, " ");
			time_toString(&it->startTime, ba);
		}
		bb_addString(ba, "]");
	}
	bb_addString(ba, "]");
	return 0;
}

PUBLIC int attr_activityCalendarToString(gxActivityCalendar *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba = { 0 };
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if ((ret = bb_attachString(&ba, bb_toString(&object->calendarNameActive))) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_seasonProfileToString(&object->seasonProfileActive, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_weekProfileToString(&object->weekProfileTableActive, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_dayProfileToString(&object->dayProfileTableActive, &ba)) != 0) {
		goto end;
	}

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_attachString(&ba, bb_toString(&object->calendarNamePassive))) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_seasonProfileToString(&object->seasonProfilePassive, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_weekProfileToString(&object->weekProfileTablePassive, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = obj_dayProfileToString(&object->dayProfileTablePassive, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = time_toString(&object->time, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
PUBLIC int attr_securitySetupToString(gxSecuritySetup *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->securityPolicy);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->securitySuite);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toHexString(&object->serverSystemTitle));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toHexString(&object->clientSystemTitle));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
PUBLIC int attr_hdlcSetupToString(gxIecHdlcSetup *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->communicationSpeed);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->windowSizeTransmit);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->windowSizeReceive);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->maximumInfoLengthTransmit);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->maximumInfoLengthReceive);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->interCharachterTimeout);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->inactivityTimeout);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->deviceAddress);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);

	return 0;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP

#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
PUBLIC int attr_localPortSetupToString(gxLocalPortSetup *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->defaultMode);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->defaultBaudrate);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->proposedBaudrate);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->responseTime);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toString(&object->deviceAddress));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toString(&object->password1));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toString(&object->password2));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toString(&object->password5));
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
PUBLIC int attr_IecTwistedPairSetupToString(gxIecTwistedPairSetup *object, object_attributes *attributes)
{
	int ret = 0;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if ((ret = bb_addIntAsString(&ba, object->mode)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, (int)object->speed)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (long unsigned pos = 0; pos != object->primaryAddresses.size; ++pos) {
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		if ((ret = bb_addIntAsString(&ba, object->primaryAddresses.data[pos])) != 0) {
			break;
		}
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (long unsigned pos = 0; pos != object->tabis.size; ++pos) {
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		if ((ret = bb_addIntAsString(&ba, object->tabis.data[pos])) != 0) {
			break;
		}
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
PUBLIC int attr_demandRegisterToString(gxDemandRegister *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if ((ret = var_toString(&object->currentAverageValue, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = var_toString(&object->lastAverageValue, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	// TODO: probably needs better formatting
	bb_addString(&ba, "\"Scaler\": \"");
	bb_addDoubleAsString(&ba, hlp_getScaler(object->scaler));
	bb_addString(&ba, "\", \"Unit\": \"");
	bb_addString(&ba, obj_getUnitAsString(object->unit));
	bb_addString(&ba, "\"");

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = var_toString(&object->status, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = time_toString(&object->captureTime, &ba)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->period)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->numberOfPeriods)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
PUBLIC int attr_registerActivationToString(gxRegisterActivation *object, object_attributes *attributes)
{
	int ret = 0;
	uint16_t pos;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
	gxObjectDefinition *od;
	gxKey *it;
#else
	gxObject *od;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	gxRegisterActivationMask *it;
#else
	gxKey *it;
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif //DLMS_IGNORE_OBJECT_POINTERS
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->registerAssignment.size; ++pos) {
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) &&                                 \
	!defined(DLMS_COSEM_EXACT_DATA_TYPES)
		ret = oa_getByIndex(&object->registerAssignment, pos, &od);
#else
		ret = arr_getByIndex(&object->registerAssignment, pos, (void **)&od);
#endif //DLMS_IGNORE_OBJECT_POINTERS
		if (ret != 0) {
			break;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		if ((ret = bb_addIntAsString(&ba, od->objectType)) != 0 ||
		    (ret = bb_addString(&ba, " ")) != 0 ||
		    (ret = hlp_appendLogicalName(&ba, od->logicalName)) != 0) {
			break;
		}
	}
	if (ret == 0) {
		bb_addString(&ba, "]");
		attributes->values[attributes->count++] = bb_toString(&ba);
		bb_empty(&ba);

		if ((ret = bb_addString(&ba, "[")) == 0) {
			for (pos = 0; pos != object->maskList.size; ++pos) {
				ret = arr_getByIndex(&object->maskList, pos, (void **)&it);
				if (ret != 0) {
					break;
				}
				if (pos != 0) {
					bb_addString(&ba, ", ");
				}
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) &&                                 \
	!defined(DLMS_COSEM_EXACT_DATA_TYPES)
				bb_attachString(&ba, bb_toString((gxByteBuffer *)it->key));
				bb_addString(&ba, " ");
				bb_attachString(&ba, bb_toString((gxByteBuffer *)it->value));
#else
				if ((ret = bb_attachString(&ba, bb_toHexString(&it->name))) != 0 ||
				    (ret = bb_addString(&ba, ": ")) != 0 ||
				    (ret = bb_attachString(&ba, bb_toHexString(&it->indexes))) != 0) {
					break;
				}
#endif //DLMS_IGNORE_OBJECT_POINTERS
			}
			bb_addString(&ba, "]");
			attributes->values[attributes->count++] = bb_toString(&ba);
		}
	}

	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION

#ifndef DLMS_IGNORE_REGISTER_MONITOR
PUBLIC int attr_registerMonitorToString(gxRegisterMonitor *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos;
	dlmsVARIANT *tmp;
	gxByteBuffer ba;
	gxActionSet *as;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->thresholds.size; ++pos) {
		ret = va_getByIndex(&object->thresholds, pos, &tmp);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		ret = var_toString(tmp, &ba);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
		if (ret != 0) {
			return DLMS_ERROR_CODE_INVALID_RESPONSE;
		}
#endif
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

#ifndef DLMS_IGNORE_OBJECT_POINTERS
	if (object->monitoredValue.target == NULL) {
		hlp_appendLogicalName(&ba, EMPTY_LN);
		bb_addString(&ba, " ");
		bb_addString(&ba, obj_typeToString2(0));
	} else {
		hlp_appendLogicalName(&ba, object->monitoredValue.target->logicalName);
		bb_addString(&ba, " ");
		bb_addString(&ba, obj_typeToString2(object->monitoredValue.target->objectType));
	}
#else
	hlp_appendLogicalName(&ba, object->monitoredValue.logicalName);
	bb_addString(&ba, " ");
	bb_addString(&ba, obj_typeToString2(object->monitoredValue.objectType));
#endif //DLMS_IGNORE_OBJECT_POINTERS
	bb_addString(&ba, " ");
	bb_addIntAsString(&ba, object->monitoredValue.attributeIndex);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->actions.size; ++pos) {
		ret = arr_getByIndex(&object->actions, pos, (void **)&as);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		actionItemToString(&as->actionUp, &ba);
		bb_addString(&ba, " ");
		actionItemToString(&as->actionDown, &ba);
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
PUBLIC int attr_actionScheduleToString(gxActionSchedule *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos;
	gxtime *tm;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

#ifndef DLMS_IGNORE_OBJECT_POINTERS
	if (object->executedScript == NULL) {
		hlp_appendLogicalName(&ba, EMPTY_LN);
	} else {
		hlp_appendLogicalName(&ba, object->executedScript->base.logicalName);
	}
#else
	hlp_appendLogicalName(&ba, object->executedScriptLogicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS

	bb_addString(&ba, " ");
	bb_addIntAsString(&ba, object->executedScriptSelector);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->type);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->executionTime.size; ++pos) {
		ret = arr_getByIndex(&object->executionTime, pos, (void **)&tm);
		if (ret != DLMS_ERROR_CODE_OK) {
			goto end;
		} else {
			if (pos != 0) {
				bb_addString(&ba, ", ");
			}
			ret = time_toString(tm, &ba);
			if (ret != 0) {
				goto end;
			}
		}
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE

#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
PUBLIC int attr_sapAssignmentToString(gxSapAssignment *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos;
	gxSapItem *it;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->sapAssignmentList.size; ++pos) {
		ret = arr_getByIndex(&object->sapAssignmentList, pos, (void **)&it);
		if (ret != 0) {
			return ret;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_addIntAsString(&ba, it->id);
		bb_addString(&ba, ": ");
		bb_set2(&ba, &it->name, 0, bb_size(&it->name));
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
PUBLIC int attr_autoAnswerToString(gxAutoAnswer *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;

	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if ((ret = bb_addIntAsString(&ba, object->mode)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addString(&ba, "[")) != 0) {
		goto end;
	}

	if ((ret = obj_timeWindowToString(&object->listeningWindow, &ba)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, "]")) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->status)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->numberOfCalls)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->numberOfRingsInListeningWindow)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, " ")) != 0) {
		goto end;
	}

	if ((ret = bb_addIntAsString(&ba, object->numberOfRingsOutListeningWindow)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);

end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_IP4_SETUP
PUBLIC int attr_ip4SetupToString(gxIp4Setup *object, object_attributes *attributes)
{
	int ret;
	uint16_t pos;
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) &&                                 \
	!defined(DLMS_COSEM_EXACT_DATA_TYPES)
	dlmsVARIANT *tmp;
#else
	uint32_t *tmp;
#endif
	gxip4SetupIpOption *ip;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

#ifndef DLMS_IGNORE_OBJECT_POINTERS
	if (object->dataLinkLayer != NULL) {
		char ln[12] = { 0 };
		hlp_getLogicalNameToString(object->dataLinkLayer->logicalName, ln);
		bb_addString(&ba, ln);
		free(object->dataLinkLayer);
	}
#else
	bb_addLogicalName(&ba, object->dataLinkLayerReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->ipAddress);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->multicastIPAddress.size; ++pos) {
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) &&                                 \
	!defined(DLMS_COSEM_EXACT_DATA_TYPES)
		ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp);
#else
		ret = arr_getByIndex2(&object->multicastIPAddress, pos, (void **)&tmp, sizeof(uint32_t));
#endif
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
		ret = bb_addIntAsString(&ba, *tmp);
#else
		ret = var_toString(tmp, &ba);
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
		if (ret != 0) {
			goto end;
		}
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->ipOptions.size; ++pos) {
		ret = arr_getByIndex(&object->ipOptions, pos, (void **)&ip);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_addIntAsString(&ba, ip->type);
		bb_addString(&ba, " ");
		bb_attachString(&ba, bb_toString(&ip->data));
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->subnetMask);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->gatewayIPAddress);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->useDHCP);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->primaryDNSAddress);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->secondaryDNSAddress);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	// var_toString(&object->value, &ba);
	// attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
PUBLIC int attr_getIPAddress(gxByteBuffer *ba, gxArray *arr)
{
	char tmp[64];
	int ret;
	uint16_t pos;
	IN6_ADDR *ip;
	if ((ret = bb_addString(ba, "{")) == 0) {
		for (pos = 0; pos != arr->size; ++pos) {
			if ((ret = arr_getByIndex(arr, pos, (void **)&ip)) != 0) {
				break;
			}
			if (pos != 0) {
				bb_addString(ba, ", ");
			}
			//Add Ws2_32.lib for LabWindows/CVI.
			inet_ntop(AF_INET6, &ip, tmp, sizeof(tmp));
			bb_addString(ba, tmp);
		}
		if (ret == 0) {
			ret = bb_addString(ba, "}");
		}
	}
	return ret;
}

PUBLIC int attr_getNeighborDiscoverySetupAsString(gxByteBuffer *ba, gxArray *arr)
{
	int ret;
	uint16_t pos;
	gxNeighborDiscoverySetup *it;
	if ((ret = bb_addString(ba, "{")) == 0) {
		for (pos = 0; pos != arr->size; ++pos) {
			if ((ret = arr_getByIndex(arr, pos, (void **)&it)) != 0) {
				break;
			}
			if (pos != 0) {
				bb_addString(ba, ", ");
			}
			if ((ret = bb_addString(ba, "[")) != 0 ||
			    (ret = bb_addIntAsString(ba, it->maxRetry)) != 0 ||
			    (ret = bb_addString(ba, ", ")) != 0 ||
			    (ret = bb_addIntAsString(ba, it->retryWaitTime)) != 0 ||
			    (ret = bb_addString(ba, ", ")) != 0 ||
			    (ret = bb_addIntAsString(ba, it->sendPeriod)) != 0 ||
			    (ret = bb_addString(ba, "]")) != 0) {
				break;
			}
		}
		if (ret == 0) {
			ret = bb_addString(ba, "}");
		}
	}
	return ret;
}

PUBLIC int attr_ip6SetupToString(gxIp6Setup *object, object_attributes *attributes)
{
	char tmp[64];
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

#ifndef DLMS_IGNORE_OBJECT_POINTERS
	if (object->dataLinkLayer != NULL) {
		char ln[12] = { 0 };
		hlp_getLogicalNameToString(object->dataLinkLayer->logicalName, ln);
		bb_addString(&ba, ln);
		free(object->dataLinkLayer);
	}
#else
	bb_addLogicalName(&ba, object->dataLinkLayerReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addIntAsString(&ba, object->addressConfigMode)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addString(&ba, "[")) != 0) {
		goto end;
	}

	if ((ret = obj_getIPAddress(&ba, &object->unicastIPAddress)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, "]")) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addString(&ba, "[")) != 0) {
		goto end;
	}

	if ((ret = obj_getIPAddress(&ba, &object->multicastIPAddress)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, "]")) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if ((ret = bb_addString(&ba, "[")) != 0) {
		goto end;
	}

	if ((ret = obj_getIPAddress(&ba, &object->gatewayIPAddress)) != 0) {
		goto end;
	}

	if ((ret = bb_addString(&ba, "]")) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	inet_ntop(AF_INET6, &object->primaryDNSAddress, tmp, sizeof(tmp));
	bb_addString(&ba, tmp);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);
	inet_ntop(AF_INET6, &object->secondaryDNSAddress, tmp, sizeof(tmp));
	bb_addString(&ba, tmp);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);
	if ((ret = bb_addIntAsString(&ba, object->trafficClass)) != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = obj_getNeighborDiscoverySetupAsString(&ba, &object->neighborDiscoverySetup);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
PUBLIC int attr_UtilityTablesToString(gxUtilityTables *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->tableId);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, bb_size(&object->buffer));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, bb_toHexString(&object->buffer));
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_IMAGE_TRANSFER
PUBLIC int attr_imageTransferToString(gxImageTransfer *object, object_attributes *attributes)
{
	uint16_t pos;
	int ret;
	gxImageActivateInfo *it;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->imageBlockSize);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_attachString(&ba, ba_toString(&object->imageTransferredBlocksStatus));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->imageFirstNotTransferredBlockNumber);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->imageTransferEnabled);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->imageTransferStatus);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->imageActivateInfo.size; ++pos) {
		ret = arr_getByIndex(&object->imageActivateInfo, pos, (void **)&it);
		if (ret != 0) {
			return ret;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_addIntAsString(&ba, it->size);
		bb_addString(&ba, " ");
		bb_attachString(&ba, bb_toHexString(&it->identification));
		bb_addString(&ba, " ");
		bb_attachString(&ba, bb_toHexString(&it->signature));
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
PUBLIC int attr_disconnectControlToString(gxDisconnectControl *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->outputState);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->controlState);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->controlMode);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
PUBLIC int attr_limiterToString(gxLimiter *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	if (object->monitoredValue != NULL) {
		hlp_appendLogicalName(&ba, object->monitoredValue->logicalName);
		bb_addString(&ba, ": ");
		bb_addIntAsString(&ba, object->selectedAttributeIndex);
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = var_toString(&object->thresholdActive, &ba);
	if (ret != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = var_toString(&object->thresholdNormal, &ba);
	if (ret != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = var_toString(&object->thresholdEmergency, &ba);
	if (ret != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->minOverThresholdDuration);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->minUnderThresholdDuration);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->emergencyProfile.id);
	bb_addString(&ba, " ");
	time_toString(&object->emergencyProfile.activationTime, &ba);
	bb_addString(&ba, " ");
	bb_addIntAsString(&ba, object->emergencyProfile.duration);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	if ((ret = obj_UInt16ArrayToString(&ba, &object->emergencyProfileGroupIDs)) != 0) {
		goto end;
	}
#else
	if ((ret = va_toString(&object->emergencyProfileGroupIDs, &ba)) != 0) {
		goto end;
	}
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->emergencyProfileActive);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	actionItemToString(&object->actionOverThreshold, &ba);
	bb_addString(&ba, " ");
	actionItemToString(&object->actionUnderThreshold, &ba);
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_LIMITER

#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
PUBLIC int attr_modemConfigurationToString(gxModemConfiguration *object, object_attributes *attributes)
{
	uint16_t pos;
	int ret;
	gxModemInitialisation *mi;
	gxByteBuffer ba, *it;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addIntAsString(&ba, object->communicationSpeed);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->initialisationStrings.size; ++pos) {
		ret = arr_getByIndex(&object->initialisationStrings, pos, (void **)&mi);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_attachString(&ba, bb_toString(&mi->request));
		bb_addString(&ba, " ");
		bb_attachString(&ba, bb_toString(&mi->response));
		bb_addString(&ba, " ");
		bb_addIntAsString(&ba, mi->delay);
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->modemProfile.size; ++pos) {
		ret = arr_getByIndex(&object->modemProfile, pos, (void **)&it);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_attachString(&ba, bb_toString(it));
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
char *parse_mac_address(char *str);
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
PUBLIC int attr_macAddressSetupToString(gxMacAddressSetup *object, object_attributes *attributes)
{
	gxByteBuffer ba = { 0 };
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_attachString(&ba, bb_toHexString(&object->macAddress));
	attributes->values[attributes->count++] = parse_mac_address(bb_toString(&ba));

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

char *parse_mac_address(char *str)
{
	int i = 0;
	while(str[i]) {
		if (str[i] == ' ') {
			str[i] = ':';
		}
		i++;
	}

	return str;
}

#ifndef DLMS_IGNORE_GPRS_SETUP
PRIVATE void qualityOfServiceToString(gxQualityOfService* target, gxByteBuffer* ba)
{
    bb_addIntAsString(ba, target->precedence);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->delay);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->reliability);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->peakThroughput);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->meanThroughput);
    bb_addString(ba, " ");
}

PUBLIC int attr_GPRSSetupToString(gxGPRSSetup *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_attachString(&ba, bb_toString(&object->apn));
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->pinCode);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	qualityOfServiceToString(&object->defaultQualityOfService, &ba);
	bb_addString(&ba, ", ");
	qualityOfServiceToString(&object->requestedQualityOfService, &ba);
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_EXTENDED_REGISTER
PUBLIC int attr_extendedRegisterToString(gxExtendedRegister *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	ret = var_toString(&object->value, &ba);
	if (ret != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	// TODO: probably needs better formatting
	bb_addString(&ba, "\"Scaler\": \"");
	bb_addIntAsString(&ba, object->scaler);
	bb_addString(&ba, "\", \"Unit\": \"");
	bb_addString(&ba, obj_getUnitAsString(object->unit));
	bb_addString(&ba, "\"");

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	ret = var_toString(&object->status, &ba);
	if (ret != 0) {
		goto end;
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	time_toString(&object->captureTime, &ba);
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

PUBLIC int attr_objectsToString(gxByteBuffer *ba, objectArray *objects)
{
	uint16_t pos;
	int ret = DLMS_ERROR_CODE_OK;
	gxObject *it;
	for (pos = 0; pos != objects->size; ++pos) {
		ret = oa_getByIndex(objects, pos, &it);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		if (pos != 0) {
			bb_addString(ba, ", ");
		}
		bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->objectType));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
		if (ret != 0) {
			return DLMS_ERROR_CODE_INVALID_RESPONSE;
		}
#endif
		bb_addString(ba, " ");
		hlp_appendLogicalName(ba, it->logicalName);
	}
	return ret;
}

PUBLIC int attr_rowsToString(gxByteBuffer *ba, gxArray *buffer)
{
	dlmsVARIANT *tmp;
	variantArray *va;
	int ret;
	uint16_t r, c;
	for (r = 0; r != buffer->size; ++r) {
		ret = arr_getByIndex(buffer, r, (void **)&va);
		if (ret != DLMS_ERROR_CODE_OK) {
			return ret;
		}
		for (c = 0; c != va->size; ++c) {
			if (c != 0) {
				bb_addString(ba, ",");
			}
			ret = va_getByIndex(va, c, &tmp);
			if (ret != DLMS_ERROR_CODE_OK) {
				return ret;
			}
			ret = var_toString(tmp, ba);
			if (ret != DLMS_ERROR_CODE_OK) {
				return ret;
			}
		}
		bb_addString(ba, ";");
	}
	return 0;
}

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
PRIVATE void obj_applicationContextNameToString(gxByteBuffer* ba, gxApplicationContextName* object)
{
    hlp_appendLogicalName(ba, object->logicalName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->jointIsoCtt);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->country);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->countryName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->identifiedOrganization);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->dlmsUA);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->applicationContext);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->contextId);
}

PRIVATE void obj_xDLMSContextTypeToString(gxByteBuffer* ba, gxXDLMSContextType* object)
{
    bb_addIntAsString(ba, object->conformance);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->maxReceivePduSize);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->maxSendPduSize);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->qualityOfService);
    bb_addString(ba, " ");
    bb_attachString(ba, bb_toString(&object->cypheringInfo));
}

PRIVATE void obj_authenticationMechanismNameToString(gxByteBuffer* ba, gxAuthenticationMechanismName* object)
{
    bb_addIntAsString(ba, object->jointIsoCtt);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->country);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->countryName);
    bb_addIntAsString(ba, object->identifiedOrganization);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->dlmsUA);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->authenticationMechanismName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->mechanismId);
}

PUBLIC int attr_associationLogicalNameToString(gxAssociationLogicalName *object, object_attributes *attributes)
{
	uint16_t pos;
	int ret = 0;
	gxKey2 *it;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&ba, "[");
	obj_objectsToString(&ba, &object->objectList);
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->clientSAP);
	bb_addString(&ba, "/");
	bb_addIntAsString(&ba, object->serverSAP);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	obj_applicationContextNameToString(&ba, &object->applicationContextName);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	obj_xDLMSContextTypeToString(&ba, &object->xDLMSContextInfo);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	obj_authenticationMechanismNameToString(&ba, &object->authenticationMechanismName);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);


	if (object->secret.data) {
		bb_attachString(&ba, bb_toString(&object->secret));
	} else {
		char* buff = calloc(12 + 1, sizeof(char));
		strlcpy(buff, "Access error", 13);
		bb_attachString(&ba, buff);
	}

	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->associationStatus);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	//Security Setup Reference is from version 1.
	if (object->base.version > 0) {
		// bb_addString(&ba, "\nIndex: 9 Value: ");
#ifndef DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_OBJECT_POINTERS
		bb_addLogicalName(&ba, obj_getLogicalName((gxObject *)object->securitySetup));
#else
		bb_addLogicalName(&ba, object->securitySetupReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
		attributes->values[attributes->count++] = bb_toString(&ba);
		bb_empty(&ba);

#endif //DLMS_IGNORE_SECURITY_SETUP
	}
	if (object->base.version > 1) {
		bb_addString(&ba, "[");
		for (pos = 0; pos != object->userList.size; ++pos) {
			if ((ret = arr_getByIndex(&object->userList, pos, (void **)&it)) != 0) {
				goto end;
			}
			bb_addString(&ba, "ID: ");
			bb_addIntAsString(&ba, it->key);
			bb_addString(&ba, " Name: ");
			bb_addString(&ba, (char *)it->value);
		}
		bb_addString(&ba, "]");
		attributes->values[attributes->count++] = bb_toString(&ba);
	}
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_PROFILE_GENERIC
PUBLIC int attr_ProfileGenericToString(gxProfileGeneric *object, object_attributes *attributes)
{
	int ret;
	gxByteBuffer ba;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_empty(&ba);
	bb_addString(&ba, "[");
	attr_rowsToString(&ba, &object->buffer);
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	obj_CaptureObjectsToString(&ba, &object->captureObjects);
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->capturePeriod);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->sortMethod);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	if (object->sortObject != NULL) {
		ret = hlp_appendLogicalName(&ba, object->sortObject->logicalName);
		if (ret != 0) {
			goto end;
		}
	}
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->entriesInUse);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->profileEntries);
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
PUBLIC int attr_GsmDiagnosticToString(gxGsmDiagnostic *object, object_attributes *attributes)
{
	int ret = 0;
	uint16_t pos;
	gxAdjacentCell *it;
	gxByteBuffer ba = { 0 };
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	bb_set(&ba, object->operatorName.data, object->operatorName.size);
#else
	bb_addString(&ba, object->operatorName);
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->status);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->circuitSwitchStatus);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->packetSwitchStatus);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "\"Cell ID\": \"");
	bb_addIntAsString(&ba, object->cellInfo.cellId);
	bb_addString(&ba, "\", \"Location ID\": \"");
	bb_addIntAsString(&ba, object->cellInfo.locationId);
	bb_addString(&ba, "\", \"Signal Quality\": \"");
	bb_addIntAsString(&ba, object->cellInfo.signalQuality);
	bb_addString(&ba, "\", \"BER\": \"");
	bb_addIntAsString(&ba, object->cellInfo.ber);
	bb_addString(&ba, "\"");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addString(&ba, "[");
	for (pos = 0; pos != object->adjacentCells.size; ++pos) {
		ret = arr_getByIndex(&object->adjacentCells, pos, (void **)&it);
		if (ret != 0) {
			goto end;
		}
		if (pos != 0) {
			bb_addString(&ba, ", ");
		}
		bb_addIntAsString(&ba, it->cellId);
		bb_addString(&ba, ":");
		bb_addIntAsString(&ba, it->signalQuality);
	}
	bb_addString(&ba, "]");
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	time_toString(&object->captureTime, &ba);
	attributes->values[attributes->count++] = bb_toString(&ba);
end:
	bb_clear(&ba);
	return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
PUBLIC int attr_CompactDataToString(gxCompactData *object, object_attributes *attributes)
{
	gxByteBuffer ba;
	char *tmp;
	BYTE_BUFFER_INIT(&ba);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	tmp = bb_toHexString(&object->buffer);
	bb_addString(&ba, tmp);
	free(tmp);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	obj_CaptureObjectsToString(&ba, &object->captureObjects);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->templateId);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	tmp = bb_toHexString(&object->templateDescription);
	bb_addString(&ba, tmp);
	free(tmp);
	attributes->values[attributes->count++] = bb_toString(&ba);
	bb_empty(&ba);

	bb_addIntAsString(&ba, object->captureMethod);
	attributes->values[attributes->count++] = bb_toString(&ba);

	bb_clear(&ba);
	return 0;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_ARBITRATOR
PUBLIC int attr_ArbitratorToString(gxArbitrator *object, object_attributes *attributes)
{
	uint16_t pos;
	int ret = 0;
	gxByteBuffer bb;
	gxActionItem *it;
	bitArray *ba;
	BYTE_BUFFER_INIT(&bb);

	attributes->values[attributes->count++] = calloc(25, sizeof(char));
	hlp_getLogicalNameToString(object->base.logicalName, attributes->values[0]);

	bb_addString(&bb, "{");
	for (pos = 0; pos != object->actions.size; ++pos) {
		if (pos != 0) {
			bb_addString(&bb, ", ");
		}
		if ((ret = arr_getByIndex(&object->actions, pos, (void **)&it)) != 0) {
			break;
		}
		bb_addString(&bb, "[");
		hlp_appendLogicalName(&bb, it->script->base.logicalName);
		bb_addString(&bb, ", ");
		bb_addIntAsString(&bb, it->scriptSelector);
		bb_addString(&bb, "]");
	}
	bb_addString(&bb, "}");
	attributes->values[attributes->count++] = bb_toString(&bb);
	bb_empty(&bb);

	bb_addString(&bb, "{");
	for (pos = 0; pos != object->permissionsTable.size; ++pos) {
		if (pos != 0) {
			bb_addString(&bb, ", ");
		}
		if ((ret = arr_getByIndex(&object->permissionsTable, pos, (void **)&ba)) != 0 ||
		    (ret = ba_toString2(&bb, ba)) != 0) {
			break;
		}
	}
	ret					= bb_addString(&bb, "}");
	attributes->values[attributes->count++] = bb_toString(&bb);

	bb_clear(&bb);
	return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR

PUBLIC int attr_to_string(gxObject *object, object_attributes *attributes)
{
	int ret = 1;
	switch (object->objectType) {
#ifndef DLMS_IGNORE_DATA
	case DLMS_OBJECT_TYPE_DATA:
		ret = attr_DataToString((gxData *)object, attributes);
		break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
	case DLMS_OBJECT_TYPE_REGISTER:
		ret = attr_RegisterToString((gxRegister *)object, attributes);
		break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
	case DLMS_OBJECT_TYPE_CLOCK:
		ret = attr_clockToString((gxClock *)object, attributes);
		break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
	case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
		ret = attr_actionScheduleToString((gxActionSchedule *)object, attributes);
		break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
	case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
		ret = attr_activityCalendarToString((gxActivityCalendar *)object, attributes);
		break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
	case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
		ret = attr_associationLogicalNameToString((gxAssociationLogicalName *)object, attributes);
		break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
	case DLMS_OBJECT_TYPE_AUTO_ANSWER:
		ret = attr_autoAnswerToString((gxAutoAnswer *)object, attributes);
		break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
	case DLMS_OBJECT_TYPE_AUTO_CONNECT:
		ret = attr_autoConnectToString((gxAutoConnect *)object, attributes);
		break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
	case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
		ret = attr_demandRegisterToString((gxDemandRegister *)object, attributes);
		break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
	case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
		ret = attr_macAddressSetupToString((gxMacAddressSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
	case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
		ret = attr_extendedRegisterToString((gxExtendedRegister *)object, attributes);
		break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
	case DLMS_OBJECT_TYPE_GPRS_SETUP:
		ret = attr_GPRSSetupToString((gxGPRSSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
	case DLMS_OBJECT_TYPE_SECURITY_SETUP:
		ret = attr_securitySetupToString((gxSecuritySetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
	case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
		ret = attr_hdlcSetupToString((gxIecHdlcSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
	case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
		ret = attr_localPortSetupToString((gxLocalPortSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
	case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
		ret = attr_IecTwistedPairSetupToString((gxIecTwistedPairSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
	case DLMS_OBJECT_TYPE_IP4_SETUP:
		ret = attr_ip4SetupToString((gxIp4Setup *)object, attributes);
		break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
	case DLMS_OBJECT_TYPE_IP6_SETUP:
		ret = attr_ip6SetupToString((gxIp6Setup *)object, attributes);
		break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
	case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
		ret = attr_imageTransferToString((gxImageTransfer *)object, attributes);
		break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
	case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
		ret = attr_disconnectControlToString((gxDisconnectControl *)object, attributes);
		break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
	case DLMS_OBJECT_TYPE_LIMITER:
		ret = attr_limiterToString((gxLimiter *)object, attributes);
		break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
	case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
		ret = attr_modemConfigurationToString((gxModemConfiguration *)object, attributes);
		break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PROFILE_GENERIC
	case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
		ret = attr_ProfileGenericToString((gxProfileGeneric *)object, attributes);
		break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
	case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
		ret = attr_registerActivationToString((gxRegisterActivation *)object, attributes);
		break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
	case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
		ret = attr_registerMonitorToString((gxRegisterMonitor *)object, attributes);
		break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
	case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
		ret = attr_sapAssignmentToString((gxSapAssignment *)object, attributes);
		break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCRIPT_TABLE
	case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
		ret = attr_ScriptTableToString((gxScriptTable *)object, attributes);
		break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
	case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
		ret = attr_specialDaysTableToString((gxSpecialDaysTable *)object, attributes);
		break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
	case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
		ret = attr_TcpUdpSetupToString((gxTcpUdpSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
	case DLMS_OBJECT_TYPE_UTILITY_TABLES:
		ret = attr_UtilityTablesToString((gxUtilityTables *)object, attributes);
		break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_PUSH_SETUP
	case DLMS_OBJECT_TYPE_PUSH_SETUP:
		ret = attr_pushSetupToString((gxPushSetup *)object, attributes);
		break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
	case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
		ret = attr_GsmDiagnosticToString((gxGsmDiagnostic *)object, attributes);
		break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
	case DLMS_OBJECT_TYPE_COMPACT_DATA:
		ret = attr_CompactDataToString((gxCompactData *)object, attributes);
		break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_ARBITRATOR
	case DLMS_OBJECT_TYPE_ARBITRATOR:
		ret = attr_ArbitratorToString((gxArbitrator *)object, attributes);
		break;
#endif //DLMS_IGNORE_ARBITRATOR
	default: //Unknown type.
		ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
	}
	return ret;
}
