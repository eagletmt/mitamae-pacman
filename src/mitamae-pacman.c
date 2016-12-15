#include <alpm.h>
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

static mrb_value m_s_alpm_version(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb, alpm_version());
}

static void alpm_handle_free(mrb_state *mrb, void *ptr) {
  alpm_handle_t *handle = (alpm_handle_t *)ptr;
  if (handle != NULL) {
    alpm_release(handle);
  }
}

static const struct mrb_data_type alpm_handle_type = {"Alpm::Handle",
                                                      alpm_handle_free};

static alpm_handle_t *unwrap_alpm_handle(mrb_state *mrb, mrb_value v) {
  return (alpm_handle_t *)mrb_data_get_ptr(mrb, v, &alpm_handle_type);
}

static mrb_value make_error(mrb_state *mrb, alpm_errno_t err) {
  mrb_value argv[2];

  argv[0] = mrb_fixnum_value(err);
  argv[1] = mrb_str_new_cstr(mrb, alpm_strerror(err));
  return mrb_obj_new(
      mrb, mrb_class_get_under(mrb, mrb_module_get(mrb, "Alpm"), "Error"), 2,
      argv);
}

static mrb_value m_handle_initialize(mrb_state *mrb, mrb_value self) {
  char *root, *dbpath;
  alpm_handle_t *handle;
  alpm_errno_t err;

  mrb_get_args(mrb, "zz", &root, &dbpath);

  handle = (alpm_handle_t *)DATA_PTR(self);
  if (handle != NULL) {
    alpm_release(handle);
  }
  mrb_data_init(self, NULL, &alpm_handle_type);
  handle = alpm_initialize(root, dbpath, &err);
  if (handle == NULL) {
    mrb_exc_raise(mrb, make_error(mrb, err));
  }
  mrb_data_init(self, handle, &alpm_handle_type);

  return self;
}

static mrb_value m_handle_release(mrb_state *mrb, mrb_value self) {
  alpm_handle_t *handle = unwrap_alpm_handle(mrb, self);
  if (handle != NULL) {
    alpm_release(handle);
    mrb_data_init(self, NULL, &alpm_handle_type);
  }
  return mrb_nil_value();
}

static mrb_value m_handle_installed_p(mrb_state *mrb, mrb_value self) {
  alpm_handle_t *handle = unwrap_alpm_handle(mrb, self);
  char *name;
  alpm_db_t *db;
  alpm_pkg_t *pkg;

  mrb_get_args(mrb, "z", &name);
  if (handle == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "alpm_handle_t isn't initialized");
  }
  db = alpm_get_localdb(handle);
  pkg = alpm_db_get_pkg(db, name);
  if (pkg == NULL) {
    return mrb_false_value();
  } else {
    alpm_pkg_free(pkg);
    return mrb_true_value();
  }
}

static mrb_value m_handle_installed_version(mrb_state *mrb, mrb_value self) {
  alpm_handle_t *handle = unwrap_alpm_handle(mrb, self);
  char *name;
  alpm_db_t *db;
  alpm_pkg_t *pkg;

  mrb_get_args(mrb, "z", &name);
  if (handle == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "alpm_handle_t isn't initialized");
  }
  db = alpm_get_localdb(handle);
  pkg = alpm_db_get_pkg(db, name);
  if (pkg == NULL) {
    return mrb_nil_value();
  } else {
    mrb_value version = mrb_str_new_cstr(mrb, alpm_pkg_get_version(pkg));
    alpm_pkg_free(pkg);
    return version;
  }
}

void mrb_mitamae_pacman_gem_init(mrb_state *mrb) {
  struct RClass *alpm = mrb_define_module(mrb, "Alpm");
  struct RClass *handle =
      mrb_define_class_under(mrb, alpm, "Handle", mrb->object_class);

  mrb_define_singleton_method(mrb, (struct RObject *)alpm, "alpm_version",
                              m_s_alpm_version, MRB_ARGS_NONE());

  MRB_SET_INSTANCE_TT(handle, MRB_TT_DATA);
  mrb_define_method(mrb, handle, "initialize", m_handle_initialize,
                    MRB_ARGS_REQ(2));
  mrb_define_method(mrb, handle, "release", m_handle_release, MRB_ARGS_NONE());
  mrb_define_method(mrb, handle, "installed?", m_handle_installed_p,
                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, handle, "installed_version",
                    m_handle_installed_version, MRB_ARGS_REQ(1));
}

void mrb_mitamae_pacman_gem_final(mrb_state *mrb) {
}
