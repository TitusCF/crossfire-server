/* image.c */
int is_valid_faceset(int fsn);
void free_socket_images(void);
void read_client_images(void);
void SetFaceMode(char *buf, int len, NewSocket *ns);
void SendFaceCmd(char *buff, int len, NewSocket *ns);
void esrv_send_face(NewSocket *ns, short face_num, int nocache);
void send_image_info(NewSocket *ns, char *params);
void send_image_sums(NewSocket *ns, char *params);
/* info.c */
void flush_output_element(object *pl, Output_Buf *outputs);
void check_output_buffers(object *pl, char *buf);
void new_draw_info(int flags, int pri, object *pl, const char *buf);
void new_draw_info_format(int flags, int pri, object *pl, char *format, ...);
void new_info_map_except(int color, mapstruct *map, object *op, char *str);
void new_info_map_except2(int color, mapstruct *map, object *op1, object *op2, char *str);
void new_info_map(int color, mapstruct *map, char *str);
void clear_win_info(object *op);
void rangetostring(object *pl, char *obuf);
void set_title(object *pl, char *buf);
void magic_mapping_mark(object *pl, char *map_mark, int strength);
void draw_magic_map(object *pl);
void Log_Kill(const char *Who, const char *What, int WhatType, const char *With, int WithType);
/* init.c */
void InitConnection(NewSocket *ns, uint32 from);
void init_ericserver(void);
void free_all_newserver(void);
void free_newsocket(NewSocket *ns);
void final_free_player(player *pl);
/* item.c */
unsigned int query_flags(object *op);
void esrv_draw_look(object *pl);
void esrv_send_inventory(object *pl, object *op);
void esrv_update_item(int flags, object *pl, object *op);
void esrv_send_item(object *pl, object *op);
void esrv_del_item(player *pl, int tag);
object *esrv_get_ob_from_count(object *pl, tag_t count);
void ExamineCmd(char *buf, int len, player *pl);
void ApplyCmd(char *buf, int len, player *pl);
void LockItem(uint8 *data, int len, player *pl);
void MarkItem(uint8 *data, int len, player *pl);
void look_at(object *op, int dx, int dy);
void LookAt(char *buf, int len, player *pl);
void esrv_move_object(object *pl, tag_t to, tag_t tag, long nrof);
/* loop.c */
void RequestInfo(char *buf, int len, NewSocket *ns);
void Handle_Oldsocket(NewSocket *ns);
void HandleClient(NewSocket *ns, player *pl);
void watchdog(void);
void doeric_server(void);
/* lowlevel.c */
void SockList_Init(SockList *sl);
void SockList_AddChar(SockList *sl, char c);
void SockList_AddShort(SockList *sl, uint16 data);
void SockList_AddInt(SockList *sl, uint32 data);
void SockList_AddInt64(SockList *sl, uint64 data);
int GetInt_String(unsigned char *data);
short GetShort_String(unsigned char *data);
int SockList_ReadPacket(int fd, SockList *sl, int len);
void write_socket_buffer(NewSocket *ns);
void Write_To_Socket(NewSocket *ns, unsigned char *buf, int len);
void cs_write_string(NewSocket *ns, const char *buf, int len);
void Send_With_Handling(NewSocket *ns, SockList *msg);
void Write_String_To_Socket(NewSocket *ns, char *buf, int len);
void write_cs_stats(void);
/* metaserver.c */
void metaserver_init(void);
void metaserver_update(void);
/* request.c */
void SetUp(char *buf, int len, NewSocket *ns);
void AddMeCmd(char *buf, int len, NewSocket *ns);
void ToggleExtendedInfos(char *buf, int len, NewSocket *ns);
void AskSmooth(char *buf, int len, NewSocket *ns);
void PlayerCmd(char *buf, int len, player *pl);
void NewPlayerCmd(uint8 *buf, int len, player *pl);
void ReplyCmd(char *buf, int len, player *pl);
void VersionCmd(char *buf, int len, NewSocket *ns);
void SetSound(char *buf, int len, NewSocket *ns);
void MapRedrawCmd(char *buff, int len, player *pl);
void MapNewmapCmd(player *pl);
void MoveCmd(char *buf, int len, player *pl);
void send_query(NewSocket *ns, uint8 flags, char *text);
void esrv_update_stats(player *pl);
void esrv_new_player(player *pl, uint32 weight);
void esrv_send_animation(NewSocket *ns, short anim_num);
int getExtendedMapInfoSize(NewSocket *ns);
void draw_client_map1(object *pl);
void draw_client_map(object *pl);
void esrv_map_scroll(NewSocket *ns, int dx, int dy);
void send_plugin_custom_message(object *pl, char *buf);
void send_skill_info(NewSocket *ns, char *params);
/* sounds.c */
void play_sound_player_only(player *pl, short soundnum, sint8 x, sint8 y);
void play_sound_map(mapstruct *map, int x, int y, short sound_num);
