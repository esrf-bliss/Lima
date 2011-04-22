//=============================================================================
// GalilBitsRecords.h
//=============================================================================
// abstraction.......Galil data recors definition
// class.............several!
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _GALIL_BITS_RECORDS_H_
#define _GALIL_BITS_RECORDS_H_

//-=============================================================================
//- DEPENDENCIES
//-=============================================================================
#include <yat/bitsstream/BitsRecord.h>

//******************************************************************************
//-    SOME USEFUL TYPEDEF - SOME USEFUL TYPEDEF - SOME USEFUL TYPEDEF 
//******************************************************************************
//------------------------------------------------------------------------------
//- TYPDEDEF: AxisIdentifier
//------------------------------------------------------------------------------
typedef enum
{
  MIN_AXIS = -1,
  //-------------
  AXIS_A,
  AXIS_B,
  AXIS_C,
  AXIS_D,
  AXIS_E,
  AXIS_F,
  AXIS_G,
  AXIS_H,
  //-------------
  MAX_AXIS
} AxisIdentifier;

//------------------------------------------------------------------------------
//- TYPDEDEF: CommandIdentifier
//------------------------------------------------------------------------------
typedef enum
{
  MIN_COMMAND = -1,
  //-------------
  GEARING_CMD,
  MOTOR_OFF_CMD,
  MOTOR_ON_CMD,
  REF_POS_CMD,
  ABS_POS_CMD,
  JOG_MINUS_CMD,
  JOG_PLUS_CMD,
  STOP_CMD,
  //-------------
  MAX_COMMAND
} CommandIdentifier; 

#define NO_COMMAND MIN_COMMAND;

//=============================================================================
// GALIL AXIS NAMES (init is placed in  AxisStateManager.cpp)
//=============================================================================
extern const char * galil_axis_name[MAX_AXIS];

//=============================================================================
// GALIL CMD NAMES (init is placed in AxisStateManager.cpp)
//=============================================================================
extern const char * galil_command_name[];

// ============================================================================
// GALIL TC ERROR STRINGS (init is placed in HwStatusProxy.cpp)
// ============================================================================
extern const char * tc_error_txt[];

//******************************************************************************
//-  BR_Header - BR_Header - BR_Header - BR_Header - BR_Header - BR_Header
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: BR_Header
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_Header)
	//-- byte 0 ------------------------
  MEMBER(always_1, 1, bool);        //- This bit is always set to 1
  IGNORE_MEMBER(reserved_1, 1, bool);
  IGNORE_MEMBER(reserved_2, 1, bool);
  IGNORE_MEMBER(reserved_3, 1, bool);
  IGNORE_MEMBER(reserved_4, 1, bool);
	MEMBER(i_block_present, 1, bool);	//- Block General Inputs Present
	MEMBER(t_block_present, 1, bool);	//- Block T present ( mouvements coordonnes )
	MEMBER(s_block_present, 1, bool);	//- Block S present ( mouvements coordonnes )
	//-- byte 1 ------------------------
	MEMBER(h_block_present, 1, bool);	//- Block axe H present
	MEMBER(g_block_present, 1, bool);	//- Block axe G present
	MEMBER(f_block_present, 1, bool);	//- Block axe F present
	MEMBER(e_block_present, 1, bool);	//- Block axe E present
	MEMBER(d_block_present, 1, bool); //- Block axe D present
	MEMBER(c_block_present, 1, bool);	//- Block axe C present
	MEMBER(b_block_present, 1, bool);	//- Block axe B present
	MEMBER(a_block_present, 1, bool);	//- Block axe A present
	//- bytes 2 & 3 (word)  ------------
	MEMBER(data_record_size, 16, short); //- little Endian, byte 2 is LSB, byte 3 is MSB
END_BITS_RECORD(BR_Header)

//------------------------------------------------------------------------------
//- EXTRACTOR: BR_Header
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_Header)
  EXTRACT_MEMBER(always_1); 
  SKIP_BITS(4);		
  EXTRACT_MEMBER(i_block_present);
  EXTRACT_MEMBER(t_block_present);	
  EXTRACT_MEMBER(s_block_present);
  EXTRACT_MEMBER(h_block_present);
  EXTRACT_MEMBER(g_block_present);
  EXTRACT_MEMBER(f_block_present);	
  EXTRACT_MEMBER(e_block_present);
  EXTRACT_MEMBER(d_block_present);
  EXTRACT_MEMBER(c_block_present);
  EXTRACT_MEMBER(b_block_present);
  EXTRACT_MEMBER(a_block_present);
  EXTRACT_MEMBER(data_record_size);
END_BITS_RECORD_EXTRACTOR(BR_Header)

//------------------------------------------------------------------------------
//- DUMP: BR_Header
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_Header)
  DUMP_MEMBER(always_1);
  DUMP_SKIP_BITS(4);	
  DUMP_MEMBER(i_block_present);
  DUMP_MEMBER(t_block_present);
  DUMP_MEMBER(s_block_present);
  DUMP_MEMBER(h_block_present);
  DUMP_MEMBER(g_block_present);
  DUMP_MEMBER(f_block_present);
  DUMP_MEMBER(e_block_present);
  DUMP_MEMBER(d_block_present);
  DUMP_MEMBER(c_block_present);
  DUMP_MEMBER(b_block_present);
  DUMP_MEMBER(a_block_present);
  DUMP_MEMBER(data_record_size);
END_BITS_RECORD_DUMP(BR_Header)

//******************************************************************************
//- CLASS: Header (humanly usable mapping for BR_Header)
//******************************************************************************
class Header : public BR_Header
{
public:
  //--------------------------------------------
  Header ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~Header ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//- BR_GenIO - BR_GenIO - BR_GenIO - BR_GenIO - BR_GenIO - BR_GenIO - BR_GenIO
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: General Inputs
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_GenIO)
	//-- byte 0 ------------------------
  MEMBER(sample_num, 16, unsigned short); 
  //- 10 bytes of general input    
  IGNORE_MEMBER(gen_in_0, 8, unsigned char); //- general input 0
	IGNORE_MEMBER(gen_in_1, 8, unsigned char); //- general input 1
	IGNORE_MEMBER(gen_in_2, 8, unsigned char); //- general input 2
	IGNORE_MEMBER(gen_in_3, 8, unsigned char); //- general input 3
	IGNORE_MEMBER(gen_in_4, 8, unsigned char); //- general input 4
	IGNORE_MEMBER(gen_in_5, 8, unsigned char); //- general input 5
	IGNORE_MEMBER(gen_in_6, 8, unsigned char); //- general input 6
	IGNORE_MEMBER(gen_in_7, 8, unsigned char); //- general input 7
	IGNORE_MEMBER(gen_in_8, 8, unsigned char); //- general input 8
	IGNORE_MEMBER(gen_in_9, 8, unsigned char); //- general input 9
  //- 10 bytes of general output
	IGNORE_MEMBER(gen_out_0, 8, unsigned char); //- general output 0
	IGNORE_MEMBER(gen_out_1, 8, unsigned char); //- general output 1
	IGNORE_MEMBER(gen_out_2, 8, unsigned char); //- general output 2
	IGNORE_MEMBER(gen_out_3, 8, unsigned char); //- general output 3
	IGNORE_MEMBER(gen_out_4, 8, unsigned char); //- general output 4
	IGNORE_MEMBER(gen_out_5, 8, unsigned char); //- general output 5
	IGNORE_MEMBER(gen_out_6, 8, unsigned char); //- general output 6
	IGNORE_MEMBER(gen_out_7, 8, unsigned char); //- general output 7
	IGNORE_MEMBER(gen_out_8, 8, unsigned char); //- general output 8
	IGNORE_MEMBER(gen_out_9, 8, unsigned char); //- general output 9
  //- byte : error code
	MEMBER(err_code, 8, unsigned char);  //- error code see TC command
	 //- byte : general status information
	MEMBER(program_running, 1, bool);		 //- is ucode running
	IGNORE_MEMBER(reserved_1, 1, bool);	 //- N/A
	IGNORE_MEMBER(reserved_2, 1, bool);	 //- N/A
	IGNORE_MEMBER(reserved_3, 1, bool);	 //- N/A
	IGNORE_MEMBER(reserved_4, 1, bool);	 //- N/A
	MEMBER(wait_for_input, 1, bool);     //- N/A
	MEMBER(trace_on, 1, bool);		       //- debug trace on see command TR
	MEMBER(echo_on, 1, bool);		         //- echo On see command EO (serial only)
END_BITS_RECORD(BR_GenIO)

//------------------------------------------------------------------------------
//- EXTRACTOR: BR_GenIO
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_GenIO)
  EXTRACT_MEMBER(sample_num); 
  SKIP_BITS(160);		
  EXTRACT_MEMBER(err_code);
  EXTRACT_MEMBER(program_running);	
  SKIP_BITS(4);	
  EXTRACT_MEMBER(wait_for_input);
  EXTRACT_MEMBER(trace_on);
  EXTRACT_MEMBER(echo_on);
END_BITS_RECORD_EXTRACTOR(BR_GenIO)

//------------------------------------------------------------------------------
//- DUMP: BR_GenIO
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_GenIO)
  DUMP_MEMBER(sample_num);
  DUMP_SKIP_BITS(160);	
  DUMP_MEMBER(err_code);
  DUMP_MEMBER(program_running);
  DUMP_SKIP_BITS(4);
  DUMP_MEMBER(wait_for_input);
  DUMP_MEMBER(trace_on);
  DUMP_MEMBER(echo_on);
END_BITS_RECORD_DUMP(BR_GenIO)

//******************************************************************************
//- CLASS: GenIO (humanly usable mapping for BR_GenIO)
//******************************************************************************
class GenIO: public BR_GenIO
{
public:
  //--------------------------------------------
  GenIO ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~GenIO ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//- BR_FirmwareAxisState - BR_FirmwareAxisState - BR_FirmwareAxisState - BR_FirmwareAxisState - BR_FirmwareAxisState
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: BR_FirmwareAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_FirmwareAxisState)
  //- status word of axis byte 0
  MEMBER(negative_dir, 1, bool);  				//- move in negative direction
  MEMBER(mode_contour, 1, bool);  				//- mode of motion contour
  MEMBER(motion_slew, 1, bool); 					//- motion is slewing : derapage, acceleration,???
  MEMBER(stop_st_or_ls, 1, bool); 				//- motion is stopping due to stop command or limit switch
  MEMBER(final_ecal, 1, bool);  					//- motion is making final ecal
  MEMBER(latch_armed, 1, bool); 					//- latch is armed
  MEMBER(off_on_err_armed, 1, bool); 		  //- Off On error armed : motor off if error exceeds limit
  MEMBER(motor_off, 1, bool); 						//- Motor is powered OFF
  //- status word of axis byte 1
  MEMBER(moving, 1, bool);  		          //- move in progress
  MEMBER(mode_PA_PR, 1, bool);  					//- mode of motion Position Absolute or Position Relative
  MEMBER(mode_PA, 1, bool); 							//- mode of motion Position Absolute only
  MEMBER(FE_in_progress, 1, bool);  			//- Find Edge in progress
  MEMBER(HM_in_progress, 1, bool);  			//- HoMe in progress
  MEMBER(HM_phase_1, 1, bool);  					//- HoMe in progress phase 1
  MEMBER(HM_phase_2_or_FI, 1, bool);  		//- HoMe in progress phase 2 or Find Index
  MEMBER(mode_coord, 1, bool);  					//- axis is in coordinated motion mode
  //- axis switches byte 2
  MEMBER(latch_occured, 1, bool); 				//- a latch occured
  MEMBER(latch_state, 1, bool); 					//- state of the latch input of the axis
  IGNORE_MEMBER(reserved_1, 1, bool); 		//- N/A
  IGNORE_MEMBER(reserved_2, 1, bool); 		//- N/A
  MEMBER(no_forward_lsw, 1, bool); 				//- state of forward limit switch
  MEMBER(no_backward_lsw, 1, bool);  			//- state of backward limit switch
  MEMBER(home_sw, 1, bool); 							//- state of Home switch
  MEMBER(sm_jumper_present, 1, bool); 		//- Stepper Motor jumper is installed
  //- axis stopcode byte 3
  MEMBER(stopcode, 8, unsigned char); 	  //- axis stopcode see command SC
  //- axis reference position signed long
  MEMBER(ref_pos, 32, long); 		          //- axis reference position
  //- axis motor position signed long
  MEMBER(mot_pos, 32, long); 	            //- axis motor position see TP (encoder position)
  //- axis position error signed long
  MEMBER(pos_err, 32, long); 		          //- axis position error see TE
  //- axis auxiliary position signed long
  MEMBER(aux_pos, 32, long); 		          //- axis auxiliary encoder position see TD (motor position for steppers)
  //- axis velocity signed long
  MEMBER(velocity, 32, long);		          //- axis velocity see TV instant velocity if encoder present, 0 otherwise
  //- axis torque signed word
  MEMBER(torque, 16, short);			        //- axis torque
  //- axis analog input value signed word
  MEMBER(ana_in, 16, short);			        //- axis input value
END_BITS_RECORD(BR_FirmwareAxisState)

//------------------------------------------------------------------------------
//- EXTRACTOR: BR_FirmwareAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_FirmwareAxisState)
  EXTRACT_MEMBER(negative_dir);
  EXTRACT_MEMBER(mode_contour);
  EXTRACT_MEMBER(motion_slew); 
  EXTRACT_MEMBER(stop_st_or_ls);
  EXTRACT_MEMBER(final_ecal);  				
  EXTRACT_MEMBER(latch_armed); 					
  EXTRACT_MEMBER(off_on_err_armed); 		
  EXTRACT_MEMBER(motor_off); 

  EXTRACT_MEMBER(moving);
  EXTRACT_MEMBER(mode_PA_PR);
  EXTRACT_MEMBER(mode_PA);
  EXTRACT_MEMBER(FE_in_progress);
  EXTRACT_MEMBER(HM_in_progress);
  EXTRACT_MEMBER(HM_phase_1);
  EXTRACT_MEMBER(HM_phase_2_or_FI);
  EXTRACT_MEMBER(mode_coord);

  EXTRACT_MEMBER(latch_occured); 				
  EXTRACT_MEMBER(latch_state); 					
  SKIP_BITS(2); 						 						
  EXTRACT_MEMBER(no_forward_lsw); 					
  EXTRACT_MEMBER(no_backward_lsw);  				
  EXTRACT_MEMBER(home_sw); 							
  EXTRACT_MEMBER(sm_jumper_present);
  
  EXTRACT_MEMBER(stopcode);
  EXTRACT_MEMBER(ref_pos);
  EXTRACT_MEMBER(mot_pos);
  EXTRACT_MEMBER(pos_err);
  EXTRACT_MEMBER(aux_pos);
  EXTRACT_MEMBER(velocity);
  EXTRACT_MEMBER(torque);
  EXTRACT_MEMBER(ana_in);
END_BITS_RECORD_EXTRACTOR(BR_FirmwareAxisState)

//------------------------------------------------------------------------------
//- DUMP: BR_FirmwareAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_FirmwareAxisState)
  DUMP_MEMBER(negative_dir);
  DUMP_MEMBER(mode_contour);
  DUMP_MEMBER(motion_slew); 
  DUMP_MEMBER(stop_st_or_ls);
  DUMP_MEMBER(final_ecal);  				
  DUMP_MEMBER(latch_armed); 					
  DUMP_MEMBER(off_on_err_armed); 		
  DUMP_MEMBER(motor_off); 
  DUMP_MEMBER(moving);
  DUMP_MEMBER(mode_PA_PR);
  DUMP_MEMBER(mode_PA);
  DUMP_MEMBER(FE_in_progress);
  DUMP_MEMBER(HM_in_progress);
  DUMP_MEMBER(HM_phase_1);
  DUMP_MEMBER(HM_phase_2_or_FI);
  DUMP_MEMBER(mode_coord);
  DUMP_MEMBER(latch_occured); 				
  DUMP_MEMBER(latch_state); 					
  DUMP_SKIP_BITS(2); 						 						
  DUMP_MEMBER(no_forward_lsw); 					
  DUMP_MEMBER(no_backward_lsw);  				
  DUMP_MEMBER(home_sw); 							
  DUMP_MEMBER(sm_jumper_present); 		
  DUMP_MEMBER(stopcode);
  DUMP_MEMBER(ref_pos);
  DUMP_MEMBER(mot_pos);
  DUMP_MEMBER(pos_err);
  DUMP_MEMBER(aux_pos);
  DUMP_MEMBER(velocity);
  DUMP_MEMBER(torque);
  DUMP_MEMBER(ana_in);
END_BITS_RECORD_DUMP(BR_FirmwareAxisState)

//******************************************************************************
//- CLASS: FirmwareAxisState (humanly usable mapping for BR_FirmwareAxisState)
//******************************************************************************
class FirmwareAxisState : public BR_FirmwareAxisState
{
public:
  //--------------------------------------------
  FirmwareAxisState ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~FirmwareAxisState ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//- BR_QRBlock - BR_QRBlock - BR_QRBlock - BR_QRBlock - BR_QRBlock - BR_QRBlock
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: BR_QRBlock
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_QRBlock)
  Header gh;
  GenIO gio;
  FirmwareAxisState ab_a;
  FirmwareAxisState ab_b;
  FirmwareAxisState ab_c;
  FirmwareAxisState ab_d;
  FirmwareAxisState ab_e;
  FirmwareAxisState ab_f;
  FirmwareAxisState ab_g;
  FirmwareAxisState ab_h;
END_BITS_RECORD(BR_QRBlock)

//------------------------------------------------------------------------------
//- EXTRACTOR: for BR_QRBlock
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_QRBlock)
  EXTRACT_MEMBER(gh);
  EXTRACT_MEMBER(gio);
  EXTRACT_MEMBER(ab_a);
  EXTRACT_MEMBER(ab_b);
  EXTRACT_MEMBER(ab_c);
  EXTRACT_MEMBER(ab_d);
  EXTRACT_MEMBER(ab_e);
  EXTRACT_MEMBER(ab_f);
  EXTRACT_MEMBER(ab_g);
  EXTRACT_MEMBER(ab_h);
END_BITS_RECORD_EXTRACTOR(BR_QRBlock)

//------------------------------------------------------------------------------
//- DUMP: BR_QRBlock
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_QRBlock)
  DUMP_MEMBER(gh);
  DUMP_MEMBER(gio);  
  DUMP_MEMBER(ab_a);
  DUMP_MEMBER(ab_b);
  DUMP_MEMBER(ab_c);
  DUMP_MEMBER(ab_d);
  DUMP_MEMBER(ab_e);
  DUMP_MEMBER(ab_f);
  DUMP_MEMBER(ab_g);
  DUMP_MEMBER(ab_h);
END_BITS_RECORD_DUMP(BR_QRBlock)

//******************************************************************************
//- CLASS: QRBlock (humanly usable mapping for BR_QRBlock)
//******************************************************************************
class QRBlock: public BR_QRBlock
{
public:
  //--------------------------------------------
  QRBlock ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~QRBlock ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//- BR_UCodeAxisState - BR_UCodeAxisState - BR_UCodeAxisState - BR_UCodeAxisState
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: BR_UCodeAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_UCodeAxisState)
  //- byte 0
  MEMBER(pos_done, 1, bool); 					 //- position done, success |-/(=0 : moving)
  MEMBER(motor_ctrl_allowed, 1, bool); //- moving is autorised
  MEMBER(pwr_ctrl_allowed, 1, bool);	 //- ON/OFF is autorised
  MEMBER(specific_microcode, 1, bool); //- specific microcode is running
  MEMBER(remote_manual, 1, bool);			 //- remote manual control is active
  MEMBER(closed_loop_stepper, 1, bool);//- stepper operated in maintenance( pseudo-closed-loop) mode
  MEMBER(encoder_present, 1, bool);		 //- encoder present
  MEMBER(mot_type, 1, bool); 					 //- 0 = stepper, 1 = other (servo, piezo,...)
  //- byte 1
  MEMBER(axis_master, 1, bool);			   //- axis is operated in master mode (gearing)
  MEMBER(axis_slave, 1, bool); 			   //- axis is operated in slave mode (gearing)
  MEMBER(follow_error, 1, bool); 			 //- position following error see OE
  MEMBER(pos_err_lim, 1, bool);			   //- position error global, too much axes in position error
  MEMBER(ref_pos_err, 1, bool);			   //- position reference done, failed
  MEMBER(ref_pos_done, 1, bool); 		   //- position reference done, success
  MEMBER(ref_pos_in_progress, 1, bool);	   //- position reference seeking in progress
  MEMBER(pos_err, 1, bool);					   //- position done, error	 |
  //- byte 2
  IGNORE_MEMBER(reserved_23, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_22, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_21, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_20, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_19, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_18, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_17, 1, bool); //- N/A
  MEMBER(is_initialised, 1, bool); 		 //- valeurs imporatantes initialisees par le device
  //- byte 3
  IGNORE_MEMBER(reserved_31, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_30, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_29, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_28, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_27, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_26, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_25, 1, bool); //- N/A
  IGNORE_MEMBER(reserved_24, 1, bool); //- N/A
END_BITS_RECORD(BR_UCodeAxisState)

//------------------------------------------------------------------------------
//- EXTRACTOR: BR_UCodeAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_UCodeAxisState) 
  //- byte 0
  EXTRACT_MEMBER(pos_done);
  EXTRACT_MEMBER(motor_ctrl_allowed);
  EXTRACT_MEMBER(pwr_ctrl_allowed);
  EXTRACT_MEMBER(specific_microcode);
  EXTRACT_MEMBER(remote_manual);
  EXTRACT_MEMBER(closed_loop_stepper); 
  EXTRACT_MEMBER(encoder_present);
  EXTRACT_MEMBER(mot_type);
  //- byte 1
  EXTRACT_MEMBER(axis_master);
  EXTRACT_MEMBER(axis_slave); 			
  EXTRACT_MEMBER(follow_error);
  EXTRACT_MEMBER(pos_err_lim);
  EXTRACT_MEMBER(ref_pos_err);
  EXTRACT_MEMBER(ref_pos_done); 
  EXTRACT_MEMBER(ref_pos_in_progress);
  EXTRACT_MEMBER(pos_err);
  //- byte 2
  SKIP_BITS(7); 
  EXTRACT_MEMBER(is_initialised);
  //- byte 3
  SKIP_BITS(8); 
END_BITS_RECORD_EXTRACTOR(BR_UCodeAxisState)

//------------------------------------------------------------------------------
//- DUMP: BR_UCodeAxisState
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_UCodeAxisState) 		
  DUMP_MEMBER(pos_done);
  DUMP_MEMBER(motor_ctrl_allowed);
  DUMP_MEMBER(pwr_ctrl_allowed);
  DUMP_MEMBER(specific_microcode);
  DUMP_MEMBER(remote_manual);
  DUMP_MEMBER(closed_loop_stepper); 
  DUMP_MEMBER(encoder_present);
  DUMP_MEMBER(mot_type);
  DUMP_MEMBER(axis_master);
  DUMP_MEMBER(axis_slave); 			
  DUMP_MEMBER(follow_error);
  DUMP_MEMBER(pos_err_lim);
  DUMP_MEMBER(ref_pos_err);
  DUMP_MEMBER(ref_pos_done); 
  DUMP_MEMBER(ref_pos_in_progress);
  DUMP_MEMBER(pos_err);
  DUMP_MEMBER(is_initialised);
END_BITS_RECORD_DUMP(BR_UCodeAxisState)

//******************************************************************************
//- CLASS: UCodeAxisState (humanly usable mapping for BR_UCodeAxisState)
//******************************************************************************
class UCodeAxisState : public BR_UCodeAxisState
{
public:
  //--------------------------------------------
  UCodeAxisState ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~UCodeAxisState ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//-  BR_UCodeAxisCommand - BR_UCodeAxisCommand - BR_UCodeAxisCommand
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: BR_UCodeAxisCommand
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(BR_UCodeAxisCommand)
	 //-- byte 0
	 MEMBER(update_gearing_mode, 1, bool); //- update for gearing
	 MEMBER(set_motor_off, 1, bool);			 //- request for motor off
	 MEMBER(set_motor_on, 1, bool); 			 //- request for motor on
	 MEMBER(req_init_ref_pos, 1, bool); 	 //- request to initialise reference position
	 MEMBER(req_pos, 1, bool);						 //- request to absolute positionning
	 MEMBER(req_jog_minus, 1, bool);			 //- request jog negative direction
	 MEMBER(req_jog_plus, 1, bool); 			 //- request jog positive direction
	 MEMBER(req_stop, 1, bool);						 //- request to stop
	 //- byte 1 
	 IGNORE_MEMBER(reserved_15, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_14, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_13, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_12, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_11, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_10, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_9, 1, bool);   //- N/A
	 IGNORE_MEMBER(reserved_8, 1, bool);   //- N/A
	 //- byte 2
	 IGNORE_MEMBER(reserved_23, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_22, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_21, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_20, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_19, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_18, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_17, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_16, 1, bool);  //- N/A
	 //- byte 3
	 IGNORE_MEMBER(reserved_31, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_30, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_29, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_28, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_27, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_26, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_25, 1, bool);  //- N/A
	 IGNORE_MEMBER(reserved_24, 1, bool);  //- N/A
END_BITS_RECORD(BR_UCodeAxisCommand)   

//------------------------------------------------------------------------------
//- EXTRACTOR: BR_UCodeAxisCommand
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_EXTRACTOR(BR_UCodeAxisCommand)	
  EXTRACT_MEMBER(update_gearing_mode);
  EXTRACT_MEMBER(set_motor_off);
  EXTRACT_MEMBER(set_motor_on); 			
  EXTRACT_MEMBER(req_init_ref_pos);
  EXTRACT_MEMBER(req_pos);
  EXTRACT_MEMBER(req_jog_minus);
  EXTRACT_MEMBER(req_jog_plus); 
  EXTRACT_MEMBER(req_stop);
  SKIP_BITS(24); 
END_BITS_RECORD_EXTRACTOR(BR_UCodeAxisCommand)

//------------------------------------------------------------------------------
//- DUMP: BR_UCodeAxisCommand
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_UCodeAxisCommand)
  DUMP_MEMBER(update_gearing_mode);
  DUMP_MEMBER(set_motor_off);
  DUMP_MEMBER(set_motor_on); 			
  DUMP_MEMBER(req_init_ref_pos);
  DUMP_MEMBER(req_pos);
  DUMP_MEMBER(req_jog_minus);
  DUMP_MEMBER(req_jog_plus); 
  DUMP_MEMBER(req_stop);
END_BITS_RECORD_DUMP(BR_UCodeAxisCommand)

//******************************************************************************
//- CLASS: UCodeAxisCommandState (humanly usable mapping for BR_UCodeAxisCommand)
//******************************************************************************
class UCodeAxisCommandState : public BR_UCodeAxisCommand
{
public:
  //--------------------------------------------
  UCodeAxisCommandState ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~UCodeAxisCommandState ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
  //--------------------------------------------
  bool any_command_in_progress () const
  //--------------------------------------------
  {
    return update_gearing_mode()
        || set_motor_off()
        || set_motor_on()
        || req_init_ref_pos()
        || req_pos()
        || req_jog_minus()
        || req_jog_plus()
        || req_stop();
  }
  //--------------------------------------------
  bool command_in_progress (CommandIdentifier& cmd_id_) const
  //--------------------------------------------
  {
    if (update_gearing_mode())
    {
      cmd_id_ = GEARING_CMD;
      return true;
    }
    if (set_motor_off())
    {
      cmd_id_ = MOTOR_OFF_CMD;
      return true;
    }
    if (set_motor_on())
    {
      cmd_id_ = MOTOR_ON_CMD;
      return true;
    }
    if (req_init_ref_pos())
    {
      cmd_id_ = REF_POS_CMD;
      return true;
    }
    if (req_pos())
    {
      cmd_id_ = ABS_POS_CMD;
      return true;
    }
    if (req_jog_minus())
    {
      cmd_id_ = JOG_MINUS_CMD;
      return true;
    }
    if (req_jog_plus())
    {
      cmd_id_ = JOG_PLUS_CMD;
      return true;
    }
    if (req_stop())
    {
      cmd_id_ = STOP_CMD;
      return true;
    }
    cmd_id_ = NO_COMMAND;
    return false;
  }
  //--------------------------------------------
  const char * command_name (CommandIdentifier _cmd_id) const 
  //--------------------------------------------
  {
    return galil_command_name[_cmd_id];
  }
};

//******************************************************************************
//-  BR_UCodeStatus - BR_UCodeStatus - BR_UCodeStatus - BR_UCodeStatus
//******************************************************************************
BEGIN_BITS_RECORD(BR_UCodeStatus)
	 MEMBER(running, 1, bool);            //- running
	 MEMBER(processing_ref_pos, 1, bool);	//- currently processing ref. pos.
END_BITS_RECORD(BR_UCodeAxisCommand)

//------------------------------------------------------------------------------
//- DUMP: BR_UCodeStatus
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(BR_UCodeStatus) 		
  DUMP_MEMBER(running);
  DUMP_MEMBER(processing_ref_pos);
END_BITS_RECORD_DUMP(BR_UCodeStatus)

//******************************************************************************
//- CLASS: UCodeStatus (humanly usable mapping for BR_UCodeStatus)
//******************************************************************************
class UCodeStatus : public BR_UCodeStatus
{
public:
  //--------------------------------------------
  UCodeStatus ()
  //--------------------------------------------
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~UCodeStatus ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
};

//******************************************************************************
//- CLASS: HardwareStatus
//******************************************************************************
class HardwareStatus
{
public:
  //--------------------------------------------
  HardwareStatus ()
  //--------------------------------------------
  : tc_error(0), tc_error_str("no error")
  { 
    //-noop ctor 
  }
  //--------------------------------------------
  ~HardwareStatus ()
  //--------------------------------------------
  { 
    //-noop dtor 
  }
  //--------------------------------------------
  void operator= (const HardwareStatus& src)
  //--------------------------------------------
  { 
    tc_error = src.tc_error;
    tc_error_str = src.tc_error_str;
  }
  //--------------------------------------------
  void dump () const
  //--------------------------------------------
  { 
    std::cout << " - TC error code..." 
              << static_cast<unsigned short>(tc_error)
              << std::endl;
    std::cout << " - TC error desc..." 
              << tc_error_str
              << std::endl;
  }

  //- TC error code
  unsigned char tc_error;
  
  //- TC error string
  const char* tc_error_str;
};

//******************************************************************************
//-   ExtendedAxisStatus - ExtendedAxisStatus - ExtendedAxisStatus
//******************************************************************************
//------------------------------------------------------------------------------
//- BITS RECORD: ExtendedAxisStatus
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD(ExtendedAxisStatus)
  HardwareStatus hws;
  FirmwareAxisState fas;
  UCodeStatus ucs;
  UCodeAxisState ucas;
  UCodeAxisCommandState ucacs;
END_BITS_RECORD(ExtendedAxisStatus)

//------------------------------------------------------------------------------
//- DUMP: ExtendedAxisStatus
//------------------------------------------------------------------------------
BEGIN_BITS_RECORD_DUMP(ExtendedAxisStatus)
  DUMP_MEMBER(fas);
  DUMP_MEMBER(ucs);
  DUMP_MEMBER(ucas);
  DUMP_MEMBER(ucacs);
END_BITS_RECORD_DUMP(ExtendedAxisStatus)

#endif //-- _BITS_RECORDS_H_
