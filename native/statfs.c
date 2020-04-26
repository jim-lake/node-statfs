#include <assert.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <errno.h>

#define NAPI_EXPERIMENTAL 1
#include <node_api.h>

typedef struct {
  char* filename;
  napi_deferred deferred;
  int ret;
  int err;
  uint64_t f_type;
  uint64_t f_bsize;
  uint64_t f_blocks;
  uint64_t f_bfree;
  uint64_t f_bavail;
  uint64_t f_files;
  uint64_t f_ffree;
  uint64_t f_fsid;
  uint64_t f_namelen;
  uint64_t f_frsize;
  uint64_t f_flags;
} StatfsGetData;

void statfs_get_execute(napi_env env, void* _data) {
  StatfsGetData* data = _data;

  struct statfs buf;
  data->ret = statfs(data->filename,&buf);
  if (data->ret != 0) {
    data->err = errno;
  } else {
    data->f_type = buf.f_type;
    data->f_bsize = buf.f_bsize;
    data->f_blocks = buf.f_blocks;
    data->f_bfree = buf.f_bfree;
    data->f_bavail = buf.f_bavail;
    data->f_files = buf.f_files;
    data->f_ffree = buf.f_ffree;
    data->f_fsid = *((uint64_t *)&buf.f_fsid);
    data->f_namelen = MAXPATHLEN;
    data->f_frsize = 0;
    data->f_flags = buf.f_flags;
  }
}

void statfs_get_complete(napi_env env, napi_status status, void* _data) {
  StatfsGetData* data = _data;

  free(data->filename);

  if (data->ret != 0) {
    napi_value error;
    assert(napi_create_int32(env, data->err, &error) == napi_ok);
    assert(napi_reject_deferred(env, data->deferred, error) == napi_ok);
  } else {
    napi_value object;
    assert(napi_create_object(env, &object) == napi_ok);

    napi_value type;
    napi_value bsize;
    napi_value blocks;
    napi_value bfree;
    napi_value bavail;
    napi_value files;
    napi_value ffree;
    napi_value fsid;
    napi_value namelen;
    napi_value frsize;
    napi_value flags;
    assert(napi_create_bigint_uint64(env,data->f_type,&type) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_bsize,&bsize) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_blocks,&blocks) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_bfree,&bfree) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_bavail,&bavail) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_files,&files) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_ffree,&ffree) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_fsid,&fsid) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_namelen,&namelen) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_frsize,&frsize) == napi_ok);
    assert(napi_create_bigint_uint64(env,data->f_flags,&flags) == napi_ok);

    assert(napi_set_named_property(env,object,"type",type) == napi_ok);
    assert(napi_set_named_property(env,object,"bsize",bsize) == napi_ok);
    assert(napi_set_named_property(env,object,"blocks",blocks) == napi_ok);
    assert(napi_set_named_property(env,object,"bfree",bfree) == napi_ok);
    assert(napi_set_named_property(env,object,"bavail",bavail) == napi_ok);
    assert(napi_set_named_property(env,object,"files",files) == napi_ok);
    assert(napi_set_named_property(env,object,"ffree",ffree) == napi_ok);
    assert(napi_set_named_property(env,object,"fsid",fsid) == napi_ok);
    assert(napi_set_named_property(env,object,"namelen",namelen) == napi_ok);
    assert(napi_set_named_property(env,object,"frsize",frsize) == napi_ok);
    assert(napi_set_named_property(env,object,"flags",flags) == napi_ok);
    assert(napi_resolve_deferred(env, data->deferred, object) == napi_ok);
  }
  free(_data);
}

napi_value statfs_get(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2];
  assert(napi_get_cb_info(env, info, &argc, args, NULL, NULL) == napi_ok);

  StatfsGetData* data = malloc(sizeof(StatfsGetData));

  size_t filename_length;
  assert(napi_get_value_string_utf8(env, args[0], NULL, 0, &filename_length) == napi_ok);
  data->filename = malloc(filename_length + 1);
  assert(napi_get_value_string_utf8(env, args[0], data->filename, filename_length + 1, NULL) == napi_ok);

  napi_value promise;
  assert(napi_create_promise(env, &data->deferred, &promise) == napi_ok);

  napi_value work_name;
  assert(napi_create_string_utf8(env, "node-statfs:get", NAPI_AUTO_LENGTH, &work_name) == napi_ok);

  napi_async_work work;
  assert(napi_create_async_work(env, NULL, work_name, statfs_get_execute, statfs_get_complete, (void*) data, &work) == napi_ok);

  assert(napi_queue_async_work(env, work) == napi_ok);

  return promise;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_value result;
  assert(napi_create_object(env, &result) == napi_ok);

  napi_value get_fn;
  assert(napi_create_function(env, "statfs", NAPI_AUTO_LENGTH, statfs_get, NULL, &get_fn) == napi_ok);
  assert(napi_set_named_property(env, result, "statfs", get_fn) == napi_ok);

  return result;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

