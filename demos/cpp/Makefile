# **********************************************************************
#
# Copyright (c) ZeroC, Inc. All rights reserved.
#
# **********************************************************************

top_srcdir      := ../..
lang_srcdir     := $(top_srcdir)/demos/cpp

include $(lang_srcdir)/make/Make.rules

demos           = minimal keyFilter node sampleFilter clock stock

include $(shell find $(lang_srcdir) -name Makefile.mk)
$(foreach t,$(demos),$(eval $(call create-demo-project,$(t))))
$(call make-projects,$(projects))
