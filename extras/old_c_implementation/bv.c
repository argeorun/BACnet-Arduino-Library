#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pin_config.h"

#include "src/bacnet/bacdef.h"
#include "src/bacnet/bacdcode.h"
#include "src/bacnet/bacenum.h"
#include "src/bacnet/config.h" /* the custom stuff */
#include "src/bacnet/rp.h"
#include "src/bacnet/wp.h"
#include "src/bacnet/basic/object/bv.h"

  /* -----------------------------------------
   Object structure (keeps name to support BACnet functions)
   ----------------------------------------- */
  struct object_data {
    const uint8_t object_id;
    const char *object_name;
    uint8_t pin;
    bool is_output;
  };

  /* -----------------------------------------
   Object list: edit pins/names here
   ----------------------------------------- */
  static struct object_data Object_List[] = {
    /* Inputs (if any BV input ) */

    { 0, "D3", PIN_D3, false },
    { 1, "D4", PIN_D4, false },
    { 2, "D5", PIN_D5, false },
    { 3, "D6", PIN_D6, false },
    { 4, "D7", PIN_D7, false },

    /* outputs (if any BV output) */

    { 5, "D8", PIN_D8, true },
    { 6, "D9", PIN_D9, true },
    { 7, "D10", PIN_D10, true },
    { 8, "D11", PIN_D11, true },
    { 9, "D12", PIN_D12, true },

    { 99, "LED", PIN_LED, true }
  };

  /* number of objects */
  static const unsigned Objects_Max =
    sizeof(Object_List) / sizeof(Object_List[0]);

  /* -----------------------------------------
   Return pointer to object_data or NULL
   ----------------------------------------- */
  static struct object_data *Object_List_Element(uint32_t object_instance) {
    unsigned index;
    uint32_t object_id;

    for (index = 0; index < Objects_Max; index++) {
      object_id = Object_List[index].object_id;
      if (object_id == object_instance) {
        /* non-const pointer to static table is fine for name set operations */
        return (struct object_data *)&Object_List[index];
      }
    }
    return NULL;
  }

  /* -----------------------------------------
   BACnet API functions (kept from original)
   ----------------------------------------- */

  bool Binary_Value_Valid_Instance(uint32_t object_instance) {
    if (Object_List_Element(object_instance)) {
      return true;
    }
    return false;
  }

  unsigned Binary_Value_Count(void) {
    return Objects_Max;
  }

  uint32_t Binary_Value_Index_To_Instance(unsigned index) {
    uint32_t object_instance = UINT32_MAX;

    if (index < Objects_Max) {
      object_instance = Object_List[index].object_id;
    }

    return object_instance;
  }

  unsigned Binary_Value_Instance_To_Index(uint32_t object_instance) {
    unsigned index = 0;

    for (index = 0; index < Objects_Max; index++) {
      if (Object_List[index].object_id == object_instance) {
        break;
      }
    }

    return index;
  }

  bool Binary_Value_Name_Set(uint32_t object_instance, const char *value) {
    struct object_data *object;

    object = Object_List_Element(object_instance);
    if (object) {
      object->object_name = value;
      return true;
    }

    return false;
  }

  const char *Binary_Value_Name_ASCII(uint32_t object_instance) {
    const char *object_name = "BV-X";
    struct object_data *object;

    object = Object_List_Element(object_instance);
    if (object) {
      object_name = object->object_name;
    }

    return object_name;
  }

  /* -----------------------------------------
   Present Value (uses PIN_READ)
   ----------------------------------------- */
  BACNET_BINARY_PV Binary_Value_Present_Value(uint32_t object_instance) {
    BACNET_BINARY_PV value = BINARY_INACTIVE;
    struct object_data *object;

    object = Object_List_Element(object_instance);
    if (object) {
      if (PIN_READ(object->pin)) {
        value = BINARY_ACTIVE;
      }
    }

    return value;
  }

  /* -----------------------------------------
   Set Present Value (uses PIN_WRITE)
   ----------------------------------------- */
  bool Binary_Value_Present_Value_Set(
    uint32_t object_instance, BACNET_BINARY_PV value) {
    struct object_data *object;

    object = Object_List_Element(object_instance);
    if (object) {
      /* Only write if object is configured as output */
      if (object->is_output) {
        if (value == BINARY_ACTIVE) {
          PIN_WRITE(object->pin, true);
        } else {
          PIN_WRITE(object->pin, false);
        }
        return true;
      }
    }
    return false;
  }

  /* -----------------------------------------
   ReadProperty handler (full BACnet support)
   ----------------------------------------- */
  int Binary_Value_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata) {
    int apdu_len = 0; /* return value */
    BACNET_BIT_STRING bit_string;
    BACNET_CHARACTER_STRING char_string;
    BACNET_BINARY_PV present_value = BINARY_INACTIVE;
    uint8_t *apdu;

    apdu = rpdata->application_data;
    switch (rpdata->object_property) {
      case PROP_OBJECT_IDENTIFIER:
        apdu_len = encode_application_object_id(
          &apdu[0], OBJECT_BINARY_VALUE, rpdata->object_instance);
        break;
      case PROP_OBJECT_NAME:
        characterstring_init_ansi(
          &char_string, Binary_Value_Name_ASCII(rpdata->object_instance));
        apdu_len =
          encode_application_character_string(&apdu[0], &char_string);
        break;
      case PROP_OBJECT_TYPE:
        apdu_len =
          encode_application_enumerated(&apdu[0], OBJECT_BINARY_VALUE);
        break;
      case PROP_PRESENT_VALUE:
        present_value = Binary_Value_Present_Value(rpdata->object_instance);
        apdu_len = encode_application_enumerated(&apdu[0], present_value);
        break;
      case PROP_STATUS_FLAGS:
        /* note: see the details in the standard on how to use these */
        bitstring_init(&bit_string);
        bitstring_set_bit(&bit_string, STATUS_FLAG_IN_ALARM, false);
        bitstring_set_bit(&bit_string, STATUS_FLAG_FAULT, false);
        bitstring_set_bit(&bit_string, STATUS_FLAG_OVERRIDDEN, false);
        bitstring_set_bit(&bit_string, STATUS_FLAG_OUT_OF_SERVICE, false);
        apdu_len = encode_application_bitstring(&apdu[0], &bit_string);
        break;
      case PROP_EVENT_STATE:
        /* note: see the details in the standard on how to use this */
        apdu_len =
          encode_application_enumerated(&apdu[0], EVENT_STATE_NORMAL);
        break;
      case PROP_OUT_OF_SERVICE:
        apdu_len = encode_application_boolean(&apdu[0], false);
        break;
      default:
        rpdata->error_class = ERROR_CLASS_PROPERTY;
        rpdata->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
        apdu_len = BACNET_STATUS_ERROR;
        break;
    }
    /* only array properties can have array options */
    if ((apdu_len >= 0) && (rpdata->array_index != BACNET_ARRAY_ALL)) {
      rpdata->error_class = ERROR_CLASS_PROPERTY;
      rpdata->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
      apdu_len = BACNET_STATUS_ERROR;
    }

    return apdu_len;
  }

  /* -----------------------------------------
   WriteProperty handler (full BACnet support)
   ----------------------------------------- */
  bool Binary_Value_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data) {
    bool status = false; /* return value */
    int len = 0;
    BACNET_APPLICATION_DATA_VALUE value = { 0 };

    if (!Binary_Value_Valid_Instance(wp_data->object_instance)) {
      wp_data->error_class = ERROR_CLASS_OBJECT;
      wp_data->error_code = ERROR_CODE_UNKNOWN_OBJECT;
      return false;
    }
    /* decode some of the request */
    len = bacapp_decode_application_data(
      wp_data->application_data, wp_data->application_data_len, &value);
    /* FIXME: len < application_data_len: more data? */
    if (len < 0) {
      /* error while decoding - a value larger than we can handle */
      wp_data->error_class = ERROR_CLASS_PROPERTY;
      wp_data->error_code = ERROR_CODE_VALUE_OUT_OF_RANGE;
      return false;
    }
    /* Note: Priority array not implemented, but we accept priority parameter for Present_Value writes */
    switch (wp_data->object_property) {
      case PROP_PRESENT_VALUE:
        /* Accept writes with or without priority parameter (priority is ignored) */
        if (value.tag == BACNET_APPLICATION_TAG_ENUMERATED) {
          if ((value.type.Enumerated == BINARY_ACTIVE) || (value.type.Enumerated == BINARY_INACTIVE)) {
            status = Binary_Value_Present_Value_Set(
              wp_data->object_instance,
              (BACNET_BINARY_PV)value.type.Enumerated);
            if (!status) {
              wp_data->error_class = ERROR_CLASS_PROPERTY;
              wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            }
          } else {
            wp_data->error_class = ERROR_CLASS_PROPERTY;
            wp_data->error_code = ERROR_CODE_VALUE_OUT_OF_RANGE;
          }
        } else {
          wp_data->error_class = ERROR_CLASS_PROPERTY;
          wp_data->error_code = ERROR_CODE_INVALID_DATA_TYPE;
        }
        break;
      case PROP_OUT_OF_SERVICE:
      case PROP_OBJECT_IDENTIFIER:
      case PROP_OBJECT_NAME:
      case PROP_OBJECT_TYPE:
      case PROP_STATUS_FLAGS:
      case PROP_EVENT_STATE:
        /* These properties don't support array indexing */
        if (wp_data->array_index != BACNET_ARRAY_ALL) {
          wp_data->error_class = ERROR_CLASS_PROPERTY;
          wp_data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        } else {
          wp_data->error_class = ERROR_CLASS_PROPERTY;
          wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
        }
        break;
      default:
        /* Unknown properties don't support array indexing */
        if (wp_data->array_index != BACNET_ARRAY_ALL) {
          wp_data->error_class = ERROR_CLASS_PROPERTY;
          wp_data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        } else {
          wp_data->error_class = ERROR_CLASS_PROPERTY;
          wp_data->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
        }
        break;
    }

    return status;
  }

  /* -----------------------------------------
   Initialize pins for all objects
   ----------------------------------------- */
  void Binary_Value_Init(void) {
    unsigned index = 0;

    for (index = 0; index < Objects_Max; index++) {
      /* Use pin layer instead of AVR registers */
      if (Object_List[index].is_output) {
        PIN_INIT(Object_List[index].pin, true);
        /* ensure output is low after init */
        PIN_WRITE(Object_List[index].pin, false);
      } else {
        PIN_INIT(Object_List[index].pin, false);
      }
    }
  }

#ifdef __cplusplus
}
#endif
