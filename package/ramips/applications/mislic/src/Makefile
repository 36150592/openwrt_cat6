CC=gcc
CFLAGS += -c  -Wall -Werror \
		-Iapi_lib/includes/ \
		-Iapi_lib/vp886_api/ \
		-Iapps/common/ \
		-Iapps/tzprofile/ \
		-Iarch/ \
		-Itzlib/base/inc/ \
		-Itzlib/custom/inc/
		
LDFLAGS += -ldl -lpthread

OBJS = 	api_lib/common/vp_api_common.o \
		api_lib/common/vp_api_config.o \
		api_lib/common/vp_api_control.o \
		api_lib/common/vp_api_cslac_seq.o \
		api_lib/common/vp_api_init.o \
		api_lib/common/vp_api_query.o \
		api_lib/common/vp_api_test.o \
		api_lib/common/vp_debug.o \
		api_lib/common/vp_dtmf_detect.o \
		api_lib/common/vp_pulse_decode.o \
		api_lib/common/vp_timer_queue.o \
		api_lib/vp886_api/vp886_adaptive_ringing.o \
		api_lib/vp886_api/vp886_calibration_common.o \
		api_lib/vp886_api/vp886_common.o \
		api_lib/vp886_api/vp886_control.o \
		api_lib/vp886_api/vp886_events.o \
		api_lib/vp886_api/vp886_init.o \
		api_lib/vp886_api/vp886_query.o \
		api_lib/vp886_api/vp886_seq.o \
		api_lib/vp886_api/vp886_slac.o \
		api_lib/vp886_api/vp886_timers.o \
		apps/common/sdk_qs_board.o \
		apps/common/sdk_qs_pin_ctrl.o \
		apps/common/sdk_qs_utils.o \
		apps/tzprofile/ZLR964122_SM2_LV_BB_profiles.o \
		arch/sys_service.o \
		arch/vp_hal.o \
		tzlib/base/src/tz_base.o \
		tzlib/base/src/tz_socket.o \
		apps/ZL880_initialization/ZL880_initialization.o 
		
#		apps/ZL880_Test/ZL880_miSLIC_mpitest.o
#		apps/ZL880_initialization/ZL880_initialization.o 
		
EXEC=mislic
		
all:$(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	
clean:
	rm *.o -f
	rm $(EXEC) -f

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	
romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
