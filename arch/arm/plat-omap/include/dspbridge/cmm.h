/*
 * cmm.h
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * The Communication Memory Management(CMM) module provides shared memory
 * management services for DSP/BIOS Bridge data streaming and messaging.
 * Multiple shared memory segments can be registered with CMM. Memory is
 * coelesced back to the appropriate pool when a buffer is freed.
 *
 * The CMM_Xlator[xxx] functions are used for node messaging and data
 * streaming address translation to perform zero-copy inter-processor
 * data transfer(GPP<->DSP). A "translator" object is created for a node or
 * stream object that contains per thread virtual address information. This
 * translator info is used at runtime to perform SM address translation
 * to/from the DSP address space.
 *
 * Notes:
 *   cmm_xlator_alloc_buf - Used by Node and Stream modules for SM address
 *			  translation.
 *
 * Copyright (C) 2008 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef CMM_
#define CMM_

#include <dspbridge/devdefs.h>

#include <dspbridge/cmmdefs.h>
#include <dspbridge/host_os.h>

/*
 *  ======== cmm_calloc_buf ========
 *  Purpose:
 *      Allocate memory buffers that can be used for data streaming or
 *      messaging.
 *  Parameters:
 *      hcmm_mgr:   Cmm Mgr handle.
 *      usize:     Number of bytes to allocate.
 *      pattr:     Attributes of memory to allocate.
 *      pp_buf_va:   Address of where to place VA.
 *  Returns:
 *      Pointer to a zero'd block of SM memory;
 *      NULL if memory couldn't be allocated,
 *      or if byte_size == 0,
 *  Requires:
 *      Valid hcmm_mgr.
 *      CMM initialized.
 *  Ensures:
 *      The returned pointer, if not NULL, points to a valid memory block of
 *      the size requested.
 *
 */
extern void *cmm_calloc_buf(struct cmm_object *hcmm_mgr,
			    u32 usize, struct cmm_attrs *pattrs,
			    OUT void **pp_buf_va);

/*
 *  ======== cmm_create ========
 *  Purpose:
 *      Create a communication memory manager object.
 *  Parameters:
 *      ph_cmm_mgr:	Location to store a communication manager handle on
 *      		output.
 *      hdev_obj: Handle to a device object.
 *      pMgrAttrs:  Comm mem manager attributes.
 *  Returns:
 *      DSP_SOK:        Success;
 *      DSP_EMEMORY:    Insufficient memory for requested resources.
 *      DSP_EFAIL:      Failed to initialize critical sect sync object.
 *
 *  Requires:
 *      cmm_init(void) called.
 *      ph_cmm_mgr != NULL.
 *      pMgrAttrs->ul_min_block_size >= 4 bytes.
 *  Ensures:
 *
 */
extern dsp_status cmm_create(OUT struct cmm_object **ph_cmm_mgr,
			     struct dev_object *hdev_obj,
			     IN CONST struct cmm_mgrattrs *pMgrAttrs);

/*
 *  ======== cmm_destroy ========
 *  Purpose:
 *      Destroy the communication memory manager object.
 *  Parameters:
 *      hcmm_mgr:   Cmm Mgr handle.
 *      bForce:    Force deallocation of all cmm memory immediately if set TRUE.
 *                 If FALSE, and outstanding allocations will return DSP_EFAIL
 *                 status.
 *  Returns:
 *      DSP_SOK:        CMM object & resources deleted.
 *      DSP_EFAIL:      Unable to free CMM object due to outstanding allocation.
 *      DSP_EHANDLE:    Unable to free CMM due to bad handle.
 *  Requires:
 *      CMM is initialized.
 *      hcmm_mgr != NULL.
 *  Ensures:
 *      Memory resources used by Cmm Mgr are freed.
 */
extern dsp_status cmm_destroy(struct cmm_object *hcmm_mgr, bool bForce);

/*
 *  ======== cmm_exit ========
 *  Purpose:
 *     Discontinue usage of module. Cleanup CMM module if CMM cRef reaches zero.
 *  Parameters:
 *     n/a
 *  Returns:
 *     n/a
 *  Requires:
 *     CMM is initialized.
 *  Ensures:
 */
extern void cmm_exit(void);

/*
 *  ======== cmm_free_buf ========
 *  Purpose:
 *      Free the given buffer.
 *  Parameters:
 *      hcmm_mgr:    Cmm Mgr handle.
 *      pbuf:       Pointer to memory allocated by cmm_calloc_buf().
 *      ul_seg_id:    SM segment Id used in CMM_Calloc() attrs.
 *                  Set to 0 to use default segment.
 *  Returns:
 *      DSP_SOK
 *      DSP_EFAIL
 *  Requires:
 *      CMM initialized.
 *      buf_pa != NULL
 *  Ensures:
 *
 */
extern dsp_status cmm_free_buf(struct cmm_object *hcmm_mgr,
			       void *buf_pa, u32 ul_seg_id);

/*
 *  ======== cmm_get_handle ========
 *  Purpose:
 *      Return the handle to the cmm mgr for the given device obj.
 *  Parameters:
 *      hprocessor:   Handle to a Processor.
 *      ph_cmm_mgr:	Location to store the shared memory mgr handle on
 *      		output.
 *
 *  Returns:
 *      DSP_SOK:        Cmm Mgr opaque handle returned.
 *      DSP_EHANDLE:    Invalid handle.
 *  Requires:
 *      ph_cmm_mgr != NULL
 *      hdev_obj != NULL
 *  Ensures:
 */
extern dsp_status cmm_get_handle(void *hprocessor,
				 OUT struct cmm_object **ph_cmm_mgr);

/*
 *  ======== cmm_get_info ========
 *  Purpose:
 *      Return the current SM and VM utilization information.
 *  Parameters:
 *      hcmm_mgr:     Handle to a Cmm Mgr.
 *      cmm_info_obj:    Location to store the Cmm information on output.
 *
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Invalid handle.
 *      DSP_EINVALIDARG Invalid input argument.
 *  Requires:
 *  Ensures:
 *
 */
extern dsp_status cmm_get_info(struct cmm_object *hcmm_mgr,
			       OUT struct cmm_info *cmm_info_obj);

/*
 *  ======== cmm_init ========
 *  Purpose:
 *      Initializes private state of CMM module.
 *  Parameters:
 *  Returns:
 *      TRUE if initialized; FALSE if error occured.
 *  Requires:
 *  Ensures:
 *      CMM initialized.
 */
extern bool cmm_init(void);

/*
 *  ======== cmm_register_gppsm_seg ========
 *  Purpose:
 *      Register a block of SM with the CMM.
 *  Parameters:
 *      hcmm_mgr:         Handle to a Cmm Mgr.
 *      lpGPPBasePA:     GPP Base Physical address.
 *      ul_size:          Size in GPP bytes.
 *      dwDSPAddrOffset  GPP PA to DSP PA Offset.
 *      c_factor:         Add offset if CMM_ADDTODSPPA, sub if CMM_SUBFROMDSPPA.
 *      dw_dsp_base:       DSP virtual base byte address.
 *      ul_dsp_size:       Size of DSP segment in bytes.
 *      pulSegId:        Address to store segment Id.
 *
 *  Returns:
 *      DSP_SOK:         Success.
 *      DSP_EHANDLE:     Invalid hcmm_mgr handle.
 *      DSP_EINVALIDARG: Invalid input argument.
 *      DSP_EFAIL:       Unable to register.
 *      - On success *pulSegId is a valid SM segment ID.
 *  Requires:
 *      ul_size > 0
 *      pulSegId != NULL
 *      dw_gpp_base_pa != 0
 *      c_factor = CMM_ADDTODSPPA || c_factor = CMM_SUBFROMDSPPA
 *  Ensures:
 *
 */
extern dsp_status cmm_register_gppsm_seg(struct cmm_object *hcmm_mgr,
					 unsigned int dw_gpp_base_pa,
					 u32 ul_size,
					 u32 dwDSPAddrOffset,
					 s8 c_factor,
					 unsigned int dw_dsp_base,
					 u32 ul_dsp_size,
					 u32 *pulSegId, u32 dwGPPBaseBA);

/*
 *  ======== cmm_un_register_gppsm_seg ========
 *  Purpose:
 *      Unregister the given memory segment that was previously registered
 *      by cmm_register_gppsm_seg.
 *  Parameters:
 *      hcmm_mgr:    Handle to a Cmm Mgr.
 *      ul_seg_id     Segment identifier returned by cmm_register_gppsm_seg.
 *  Returns:
 *       DSP_SOK:         Success.
 *       DSP_EHANDLE:     Invalid handle.
 *       DSP_EINVALIDARG: Invalid ul_seg_id.
 *       DSP_EFAIL:       Unable to unregister for unknown reason.
 *  Requires:
 *  Ensures:
 *
 */
extern dsp_status cmm_un_register_gppsm_seg(struct cmm_object *hcmm_mgr,
					    u32 ul_seg_id);

/*
 *  ======== cmm_xlator_alloc_buf ========
 *  Purpose:
 *      Allocate the specified SM buffer and create a local memory descriptor.
 *      Place on the descriptor on the translator's HaQ (Host Alloc'd Queue).
 *  Parameters:
 *      xlator:    Handle to a Xlator object.
 *      pVaBuf:     Virtual address ptr(client context)
 *      uPaSize:    Size of SM memory to allocate.
 *  Returns:
 *      Ptr to valid physical address(Pa) of uPaSize bytes, NULL if failed.
 *  Requires:
 *      pVaBuf != 0.
 *      uPaSize != 0.
 *  Ensures:
 *
 */
extern void *cmm_xlator_alloc_buf(struct cmm_xlatorobject *xlator,
				  void *pVaBuf, u32 uPaSize);

/*
 *  ======== cmm_xlator_create ========
 *  Purpose:
 *     Create a translator(xlator) object used for process specific Va<->Pa
 *     address translation. Node messaging and streams use this to perform
 *     inter-processor(GPP<->DSP) zero-copy data transfer.
 *  Parameters:
 *     phXlator:       Address to place handle to a new Xlator handle.
 *     hcmm_mgr:        Handle to Cmm Mgr associated with this translator.
 *     pXlatorAttrs:   Translator attributes used for the client NODE or STREAM.
 *  Returns:
 *     DSP_SOK:            Success.
 *     DSP_EINVALIDARG:    Bad input Attrs.
 *     DSP_EMEMORY:   Insufficient memory(local) for requested resources.
 *  Requires:
 *     phXlator != NULL
 *     hcmm_mgr != NULL
 *     pXlatorAttrs != NULL
 *  Ensures:
 *
 */
extern dsp_status cmm_xlator_create(OUT struct cmm_xlatorobject **phXlator,
				    struct cmm_object *hcmm_mgr,
				    struct cmm_xlatorattrs *pXlatorAttrs);

/*
 *  ======== cmm_xlator_delete ========
 *  Purpose:
 *      Delete translator resources
 *  Parameters:
 *      xlator:    handle to translator.
 *      bForce:     bForce = TRUE will free XLators SM buffers/dscriptrs.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Bad translator handle.
 *      DSP_EFAIL:      Unable to free translator resources.
 *  Requires:
 *      refs > 0
 *  Ensures:
 *
 */
extern dsp_status cmm_xlator_delete(struct cmm_xlatorobject *xlator,
				    bool bForce);

/*
 *  ======== cmm_xlator_free_buf ========
 *  Purpose:
 *      Free SM buffer and descriptor.
 *      Does not free client process VM.
 *  Parameters:
 *      xlator:    handle to translator.
 *      pBufVa      Virtual address of PA to free.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Bad translator handle.
 *  Requires:
 *  Ensures:
 *
 */
extern dsp_status cmm_xlator_free_buf(struct cmm_xlatorobject *xlator,
				      void *pBufVa);

/*
 *  ======== cmm_xlator_info ========
 *  Purpose:
 *      Set/Get process specific "translator" address info.
 *      This is used to perform fast virtaul address translation
 *      for shared memory buffers between the GPP and DSP.
 *  Parameters:
 *     xlator:     handle to translator.
 *     paddr:       Virtual base address of segment.
 *     ul_size:      Size in bytes.
 *     uSegId:      Segment identifier of SM segment(s)
 *     set_info     Set xlator fields if TRUE, else return base addr
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Bad translator handle.
 *  Requires:
 *      (refs > 0)
 *      (paddr != NULL)
 *      (ul_size > 0)
 *  Ensures:
 *
 */
extern dsp_status cmm_xlator_info(struct cmm_xlatorobject *xlator,
				  IN OUT u8 **paddr,
				  u32 ul_size, u32 uSegId, bool set_info);

/*
 *  ======== cmm_xlator_translate ========
 *  Purpose:
 *      Perform address translation VA<->PA for the specified stream or
 *      message shared memory buffer.
 *  Parameters:
 *     xlator: handle to translator.
 *     paddr    address of buffer to translate.
 *     xType    Type of address xlation. CMM_PA2VA or CMM_VA2PA.
 *  Returns:
 *     Valid address on success, else NULL.
 *  Requires:
 *      refs > 0
 *      paddr != NULL
 *      xType >= CMM_VA2PA) && (xType <= CMM_DSPPA2PA)
 *  Ensures:
 *
 */
extern void *cmm_xlator_translate(struct cmm_xlatorobject *xlator,
				  void *paddr, enum cmm_xlatetype xType);

#endif /* CMM_ */
