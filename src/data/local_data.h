#ifndef LOCAL_DATA_H
#define LOCAL_DATA_H

// @UNSURE: make 'this' a variable to allow different names

// @DOC: LOCAL_DATA_ macro usage example
//       entity_t* given to init(), update(), cleanup() needs to be called 'this'
//
//          typedef struct { int var; } local_data_t;
//          LOCAL_DATA_DECL(local_data_t, player_local_data);
//          local_data_t default_data = { .var = 10 };
//          ...
//          void player_init(entity_t* this)
//          {
//            LOCAL_DATA_INIT(local_data_t, player_local_data, default_data, data);
//            data->var += 2;
//          }
//          void player_update(entity_t* this, f32 dt)
//          {
//            LOCAL_DATA_GET(local_data_t, player_local_data, data);
//            data->var += 2;
//          }
//          void player_cleanup(entity_t* this)
//          {
//            LOCAL_DATA_DEL(player_local_data);
//          }

// @DOC: put at begining of .c file to initialize local data variables
//       used in entity init, update, cleanup, functions
//       this needs to be in place before any other LOCAL_DATA_ macros are called
//       see example at top
//       _type_: type of local data struct
//       _name_: name for the local data variables, needs to be 
//               different from other .c file local data names
#define LOCAL_DATA_DECL(_type_, _name_)                   \
    _type_* _name_##_arr               = NULL;            \
    u32   _name_##_arr_len           = 0;                 \
    /* key: entity.id, value idx for local_data_arr */    \
    hm_entry_t* _name_##_id_hm     = NULL;                \
    u32  _name_##_id_hm_len         = 0;                  \
    u32* _name_##_dead_arr     = NULL;                    \
    u32  _name_##_dead_arr_len = 0; 

// @UNSURE: @TODO: make LOCAL_DATA_SET_DEFAULT() or similar
//    typedef struct { int x; } data;
//    LOCAL_DATA_DEFAULT(data, { .x = 0  });
//    LOCAL_DATA_DELC(..., data, ...)

// @DOC: initializes a local data slot for an entity, this needs to be 
//       called before LOCAL_DATA_GET() can be called
//       see example at top
//       _type_:     type of local data struct, same as given to
//       _name_:     name given to LOCAL_DATA_DECL()
//       _data_new_: struct of type '_type_', this will be the default value
//                   given to the returned '_data_rtn_' local data
//       _data_rtn_: name for variable of type '_type_'* that can be used
//                   to access the local data after the macro has run
#define LOCAL_DATA_INIT(_type_, _name_, _data_new_, _data_rtn_)   \
  u32 _name_##idx = -1;                                           \
  _type_* _data_rtn_;                                             \
  if (_name_##_dead_arr_len > 0)                                  \
  {                                                               \
    u32 id = arrpop(_name_##_dead_arr);                           \
    _name_##_dead_arr_len--;                                      \
    _name_##idx = hmget(_name_##_id_hm, id);                      \
    _name_##_arr[_name_##idx] = _data_new_;                       \
    hm_entry_t* entry = hmgetp(_name_##_id_hm, id);               \
    entry->key = this->id;                                        \
  }                                                               \
  else                                                            \
  {                                                               \
    hmput(_name_##_id_hm, this->id, _name_##_arr_len);            \
    arrput(_name_##_arr, _data_new_);                             \
    _name_##idx = _name_##_arr_len;                               \
    _name_##_arr_len++;                                           \
  }                                                               \
  _data_rtn_ = & _name_##_arr[_name_##idx];                         

// @DOC: get local data for current entity 'this'
//       see example at top
//       _type_:     type of local data struct, same as given to
//       _name_:     name given to LOCAL_DATA_DECL()
//       _data_rtn_: name for variable of type '_type_'* that can be used
//                   to access the local data after the macro has run
#define LOCAL_DATA_GET(_type_, _name_, _data_rtn_)     \
  _type_* _data_rtn_;                                  \
  {                                                    \
    u32 idx = hmget(_name_##_id_hm, this->id);         \
    _data_rtn_ = &_name_##_arr[idx];                   \
  }                                           

// @DOC: free the local data spot occupied by 'this' up
//       see example at top
//       _name_:     name given to LOCAL_DATA_DECL()
#define LOCAL_DATA_DEL(_name_)          \
  arrput(_name_##_dead_arr, this->id);  \
  _name_##_dead_arr_len++;


#endif // LOCAL_DATA_H
