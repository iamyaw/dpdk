#   BSD LICENSE
#
#   Copyright(c) 2010-2015 Intel Corporation. All rights reserved.
#   Copyright(c) 2014-2015 6WIND S.A.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include $(RTE_SDK)/mk/internal/rte.compile-pre.mk
include $(RTE_SDK)/mk/internal/rte.install-pre.mk
include $(RTE_SDK)/mk/internal/rte.clean-pre.mk
include $(RTE_SDK)/mk/internal/rte.build-pre.mk
include $(RTE_SDK)/mk/internal/rte.depdirs-pre.mk

# VPATH contains at least SRCDIR
VPATH += $(SRCDIR)

_BUILD = $(APP)
_INSTALL = $(INSTALL-FILES-y) $(SYMLINK-FILES-y)
_INSTALL += $(RTE_OUTPUT)/app/$(APP) $(RTE_OUTPUT)/app/$(APP).map
POSTINSTALL += target-appinstall
_CLEAN = doclean
POSTCLEAN += target-appclean

ifeq ($(NO_LDSCRIPT),)
LDSCRIPT = $(RTE_LDSCRIPT)
endif

# default path for libs
LDLIBS-y += -L$(RTE_SDK_BIN)/lib

#
# Include libraries depending on config if NO_AUTOLIBS is not set
# Order is important: from higher level to lower level
#
ifeq ($(NO_AUTOLIBS),)

LDLIBS-y += --whole-archive

LDLIBS-$(CONFIG_RTE_BUILD_COMBINE_LIBS)     += -l$(RTE_LIBNAME)

ifeq ($(CONFIG_RTE_BUILD_COMBINE_LIBS),n)

LDLIBS-$(CONFIG_RTE_LIBRTE_DISTRIBUTOR)     += -lrte_distributor
LDLIBS-$(CONFIG_RTE_LIBRTE_REORDER)         += -lrte_reorder

ifeq ($(CONFIG_RTE_EXEC_ENV_LINUXAPP),y)
LDLIBS-$(CONFIG_RTE_LIBRTE_KNI)             += -lrte_kni
LDLIBS-$(CONFIG_RTE_LIBRTE_IVSHMEM)         += -lrte_ivshmem
endif

LDLIBS-$(CONFIG_RTE_LIBRTE_PIPELINE)        += -lrte_pipeline
LDLIBS-$(CONFIG_RTE_LIBRTE_TABLE)           += -lrte_table
LDLIBS-$(CONFIG_RTE_LIBRTE_PORT)            += -lrte_port
LDLIBS-$(CONFIG_RTE_LIBRTE_TIMER)           += -lrte_timer
LDLIBS-$(CONFIG_RTE_LIBRTE_HASH)            += -lrte_hash
LDLIBS-$(CONFIG_RTE_LIBRTE_JOBSTATS)        += -lrte_jobstats
LDLIBS-$(CONFIG_RTE_LIBRTE_LPM)             += -lrte_lpm
LDLIBS-$(CONFIG_RTE_LIBRTE_POWER)           += -lrte_power
LDLIBS-$(CONFIG_RTE_LIBRTE_ACL)             += -lrte_acl
LDLIBS-$(CONFIG_RTE_LIBRTE_METER)           += -lrte_meter

LDLIBS-$(CONFIG_RTE_LIBRTE_SCHED)           += -lrte_sched
LDLIBS-$(CONFIG_RTE_LIBRTE_SCHED)           += -lm
LDLIBS-$(CONFIG_RTE_LIBRTE_SCHED)           += -lrt

LDLIBS-$(CONFIG_RTE_LIBRTE_VHOST)           += -lrte_vhost

endif # ! CONFIG_RTE_BUILD_COMBINE_LIBS

LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_PCAP)        += -lpcap

ifeq ($(CONFIG_RTE_LIBRTE_VHOST_USER),n)
LDLIBS-$(CONFIG_RTE_LIBRTE_VHOST)           += -lfuse
endif

LDLIBS-$(CONFIG_RTE_LIBRTE_MLX4_PMD)        += -libverbs

LDLIBS-y += --start-group

ifeq ($(CONFIG_RTE_BUILD_COMBINE_LIBS),n)

LDLIBS-$(CONFIG_RTE_LIBRTE_KVARGS)          += -lrte_kvargs
LDLIBS-$(CONFIG_RTE_LIBRTE_MBUF)            += -lrte_mbuf
LDLIBS-$(CONFIG_RTE_LIBRTE_IP_FRAG)         += -lrte_ip_frag
LDLIBS-$(CONFIG_RTE_LIBRTE_ETHER)           += -lethdev
LDLIBS-$(CONFIG_RTE_LIBRTE_MALLOC)          += -lrte_malloc
LDLIBS-$(CONFIG_RTE_LIBRTE_MEMPOOL)         += -lrte_mempool
LDLIBS-$(CONFIG_RTE_LIBRTE_RING)            += -lrte_ring
LDLIBS-$(CONFIG_RTE_LIBRTE_EAL)             += -lrte_eal
LDLIBS-$(CONFIG_RTE_LIBRTE_CMDLINE)         += -lrte_cmdline
LDLIBS-$(CONFIG_RTE_LIBRTE_CFGFILE)         += -lrte_cfgfile
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_BOND)        += -lrte_pmd_bond

LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_XENVIRT)     += -lrte_pmd_xenvirt
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_XENVIRT)     += -lxenstore

ifeq ($(CONFIG_RTE_BUILD_SHARED_LIB),n)
# plugins (link only if static libraries)

LDLIBS-$(CONFIG_RTE_LIBRTE_VMXNET3_PMD)     += -lrte_pmd_vmxnet3_uio
LDLIBS-$(CONFIG_RTE_LIBRTE_VIRTIO_PMD)      += -lrte_pmd_virtio
LDLIBS-$(CONFIG_RTE_LIBRTE_ENIC_PMD)        += -lrte_pmd_enic
LDLIBS-$(CONFIG_RTE_LIBRTE_I40E_PMD)        += -lrte_pmd_i40e
LDLIBS-$(CONFIG_RTE_LIBRTE_FM10K_PMD)       += -lrte_pmd_fm10k
LDLIBS-$(CONFIG_RTE_LIBRTE_IXGBE_PMD)       += -lrte_pmd_ixgbe
LDLIBS-$(CONFIG_RTE_LIBRTE_E1000_PMD)       += -lrte_pmd_e1000
LDLIBS-$(CONFIG_RTE_LIBRTE_MLX4_PMD)        += -lrte_pmd_mlx4
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_RING)        += -lrte_pmd_ring
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_PCAP)        += -lrte_pmd_pcap
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_AF_PACKET)   += -lrte_pmd_af_packet
LDLIBS-$(CONFIG_RTE_LIBRTE_PMD_NULL)        += -lrte_pmd_null

endif # ! $(CONFIG_RTE_BUILD_SHARED_LIB)

endif # ! CONFIG_RTE_BUILD_COMBINE_LIBS

LDLIBS-y += $(EXECENV_LDLIBS)
LDLIBS-y += --end-group
LDLIBS-y += --no-whole-archive

endif # ifeq ($(NO_AUTOLIBS),)

LDLIBS-y += $(EXTRA_LDLIBS)

.PHONY: all
all: install

.PHONY: install
install: build _postinstall

_postinstall: build

.PHONY: build
build: _postbuild

exe2cmd = $(strip $(call dotfile,$(patsubst %,%.cmd,$(1))))

ifeq ($(LINK_USING_CC),1)
override EXTRA_LDFLAGS := $(call linkerprefix,$(EXTRA_LDFLAGS))
O_TO_EXE = $(CC) $(CFLAGS) $(LDFLAGS_$(@)) \
	-Wl,-Map=$(@).map,--cref -o $@ $(OBJS-y) $(call linkerprefix,$(LDFLAGS)) \
	$(EXTRA_LDFLAGS) $(call linkerprefix,$(LDLIBS-y))
else
O_TO_EXE = $(LD) $(LDFLAGS) $(LDFLAGS_$(@)) $(EXTRA_LDFLAGS) \
	-Map=$(@).map --cref -o $@ $(OBJS-y) $(LDLIBS-y)
endif
O_TO_EXE_STR = $(subst ','\'',$(O_TO_EXE)) #'# fix syntax highlight
O_TO_EXE_DISP = $(if $(V),"$(O_TO_EXE_STR)","  LD $(@)")
O_TO_EXE_CMD = "cmd_$@ = $(O_TO_EXE_STR)"
O_TO_EXE_DO = @set -e; \
	echo $(O_TO_EXE_DISP); \
	$(O_TO_EXE) && \
	echo $(O_TO_EXE_CMD) > $(call exe2cmd,$(@))

-include .$(APP).cmd

# path where libraries are retrieved
LDLIBS_PATH := $(subst -Wl$(comma)-L,,$(filter -Wl$(comma)-L%,$(LDLIBS-y)))
LDLIBS_PATH += $(subst -L,,$(filter -L%,$(LDLIBS-y)))

# list of .a files that are linked to this application
LDLIBS_NAMES := $(patsubst -l%,lib%.a,$(filter -l%,$(LDLIBS-y)))
LDLIBS_NAMES += $(patsubst -Wl$(comma)-l%,lib%.a,$(filter -Wl$(comma)-l%,$(LDLIBS-y)))

# list of found libraries files (useful for deps). If not found, the
# library is silently ignored and dep won't be checked
LDLIBS_FILES := $(wildcard $(foreach dir,$(LDLIBS_PATH),\
	$(addprefix $(dir)/,$(LDLIBS_NAMES))))

#
# Compile executable file if needed
#
$(APP): $(OBJS-y) $(LDLIBS_FILES) $(DEP_$(APP)) $(LDSCRIPT) FORCE
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(if $(D),\
		@echo -n "$< -> $@ " ; \
		echo -n "file_missing=$(call boolean,$(file_missing)) " ; \
		echo -n "cmdline_changed=$(call boolean,$(call cmdline_changed,$(O_TO_EXE_STR))) " ; \
		echo -n "depfile_missing=$(call boolean,$(depfile_missing)) " ; \
		echo "depfile_newer=$(call boolean,$(depfile_newer)) ")
	$(if $(or \
		$(file_missing),\
		$(call cmdline_changed,$(O_TO_EXE_STR)),\
		$(depfile_missing),\
		$(depfile_newer)),\
		$(O_TO_EXE_DO))

#
# install app in $(RTE_OUTPUT)/app
#
$(RTE_OUTPUT)/app/$(APP): $(APP)
	@echo "  INSTALL-APP $(APP)"
	@[ -d $(RTE_OUTPUT)/app ] || mkdir -p $(RTE_OUTPUT)/app
	$(Q)cp -f $(APP) $(RTE_OUTPUT)/app

#
# install app map file in $(RTE_OUTPUT)/app
#
$(RTE_OUTPUT)/app/$(APP).map: $(APP)
	@echo "  INSTALL-MAP $(APP).map"
	@[ -d $(RTE_OUTPUT)/app ] || mkdir -p $(RTE_OUTPUT)/app
	$(Q)cp -f $(APP).map $(RTE_OUTPUT)/app

#
# Clean all generated files
#
.PHONY: clean
clean: _postclean
	$(Q)rm -f $(_BUILD_TARGETS) $(_INSTALL_TARGETS) $(_CLEAN_TARGETS)

.PHONY: doclean
doclean:
	$(Q)rm -rf $(APP) $(OBJS-all) $(DEPS-all) $(DEPSTMP-all) \
	  $(CMDS-all) $(INSTALL-FILES-all) .$(APP).cmd


include $(RTE_SDK)/mk/internal/rte.compile-post.mk
include $(RTE_SDK)/mk/internal/rte.install-post.mk
include $(RTE_SDK)/mk/internal/rte.clean-post.mk
include $(RTE_SDK)/mk/internal/rte.build-post.mk
include $(RTE_SDK)/mk/internal/rte.depdirs-post.mk

ifneq ($(wildcard $(RTE_SDK)/mk/target/$(RTE_TARGET)/rte.app.mk),)
include $(RTE_SDK)/mk/target/$(RTE_TARGET)/rte.app.mk
else
include $(RTE_SDK)/mk/target/generic/rte.app.mk
endif

.PHONY: FORCE
FORCE:
