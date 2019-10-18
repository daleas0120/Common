#ifndef NET_DEFINITION_H
#define NET_DEFINITION_H


// dlib includes
#include "dlib/dnn.h"
#include "dlib/dnn/core.h"

#include "gorgon_capture.h"

#include "input_array_image_pryamid.h"

extern const uint32_t array_depth = 3;

// ----------------------------------------------------------------------------------------
template<typename SUBNET> using mp2 = dlib::max_pool<2, 2, 2, 2, SUBNET>;
template<typename SUBNET> using mp3 = dlib::max_pool<3, 3, 2, 2, SUBNET>;

// --------------------------------- Conv Filter Setup ------------------------------------
template <long num_filters, typename SUBNET> using con1 = dlib::con<num_filters, 1, 1, 1, 1, SUBNET>;
template <long num_filters, typename SUBNET> using con3 = dlib::con<num_filters, 3, 3, 1, 1, SUBNET>;
template <long num_filters, typename SUBNET> using con5 = dlib::con<num_filters, 5, 5, 1, 1, SUBNET>;
template <long num_filters, typename SUBNET> using con7 = dlib::con<num_filters, 7, 7, 1, 1, SUBNET>;
template <long num_filters, typename SUBNET> using con9 = dlib::con<num_filters, 9, 9, 1, 1, SUBNET>;

template <long num_filters, typename SUBNET> using con2d = dlib::con<num_filters, 2, 2, 2, 2, SUBNET>;
template <long num_filters, typename SUBNET> using con3d = dlib::con<num_filters, 3, 3, 2, 2, SUBNET>;
template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters, 5, 5, 2, 2, SUBNET>;
template <long num_filters, typename SUBNET> using con7d = dlib::con<num_filters, 7, 7, 2, 2, SUBNET>;
template <long num_filters, typename SUBNET> using con9d = dlib::con<num_filters, 9, 9, 2, 2, SUBNET>;


template <long num_filters, typename SUBNET> using cont2u = dlib::cont<num_filters, 2, 2, 2, 2, SUBNET>;


// --------------------------------------block definitions---------------------------------

template <int N1, int N2, int N3, typename SUBNET> using blk = dlib::bn_con<con1<N1, dlib::prelu<dlib::bn_con<con3<N2, dlib::prelu<dlib::bn_con<con1<N3, SUBNET>>>>>>>>;
template <int N1, int N2, int N3, typename SUBNET> using ablk = dlib::affine<con1<N1, dlib::prelu<dlib::affine<con3<N2, dlib::prelu<dlib::affine<con1<N3, SUBNET>>>>>>>>;
template <int N, typename SUBNET> using cbp3_blk = dlib::prelu<dlib::bn_con<con3<N, SUBNET>>>;

// --------------------------------------residual blocks-----------------------------------

template <int N1,int N2,int N3, typename SUBNET>
using res_blk = dlib::prelu<dlib::add_prev1<blk<N1,N2,N3, dlib::tag1<SUBNET>>>>;
template <int N1,int N2,int N3, typename SUBNET>
using ares_blk = dlib::prelu<dlib::add_prev1<ablk<N1,N2,N3, dlib::tag1<SUBNET>>>>;

// --------------------------------------scale blocks-----------------------------------

template <int N1,int N2,int N3, typename SUBNET>
using scale_blk1 = res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, SUBNET>>>;
template <int N1,int N2,int N3, typename SUBNET>
using ascale_blk1 = ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, SUBNET>>>;

template <int N1,int N2,int N3, typename SUBNET>
using scale_blk2 = res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, SUBNET>>>>;
template <int N1,int N2,int N3, typename SUBNET>
using ascale_blk2 = ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, SUBNET>>>>;

template <int N1,int N2,int N3, typename SUBNET>
using scale_blk3 = res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, res_blk<N1,N2,N3, SUBNET>>>>>>;
template <int N1,int N2,int N3, typename SUBNET>
using ascale_blk3 = ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, ares_blk<N1,N2,N3, SUBNET>>>>>>;

// ----------------------------------------------------------------------------------------
// Network Definition 
// ----------------------------------------------------------------------------------------

using resnet_type = dlib::loss_multiclass_log< dlib::fc<1000, dlib::avg_pool_everything<
    scale_blk1<2048,512,512,
	con2d<2048, scale_blk3<1024,256,256, 
    con2d<1024, scale_blk2<512,128,128,
    con2d<512, scale_blk1<256,64,64,
    con2d<256, dlib::prelu<dlib::bn_con<con7d<64,
    dlib::input_rgb_image_sized<230>
    >>>> >> >> >> > >>>;

using aresnet_type = dlib::loss_multiclass_log< dlib::fc<1000, dlib::avg_pool_everything<
    ascale_blk1<2048,512,512,
	con2d<2048, ascale_blk3<1024,256,256,
    con2d<1024, ascale_blk2<512,128,128,
    con2d<512, ascale_blk1<256,64,64,
    con2d<256, dlib::prelu<dlib::affine<con7d<64,
    dlib::input_rgb_image_sized<230>
    >>>> >> >> >> > >>>;
        
// ----------------------------------------------------------------------------------------
    
using ml_net_type = dlib::loss_mmod<con9<1,
    cont2u<256, cont2u<256, cont2u<256,
    //----
    scale_blk1<2048,512,512,
	con2d<2048, scale_blk3<1024,256,256, 
    con2d<1024, scale_blk2<512,128,128,
    con2d<512, scale_blk1<256,64,64,
    con2d<256, dlib::prelu<dlib::bn_con<con7d<64,    
    //----
    dlib::input_array_image_pyramid<dlib::pyramid_down<6>, array_depth>
    >>>> >> >> >> > >>> >>;
    
using aml_net_type = dlib::loss_mmod<con9<1,
    cont2u<256, cont2u<256, cont2u<256,
    //----
    ascale_blk1<2048,512,512,
	con2d<2048, ascale_blk3<1024,256,256, 
    con2d<1024, ascale_blk2<512,128,128,
    con2d<512, ascale_blk1<256,64,64,
    con2d<256, dlib::prelu<dlib::affine<con7d<64,    
    //----
    dlib::input_array_image_pyramid<dlib::pyramid_down<6>, array_depth>
    >>>> >> >> >> > >>> >>;    
    
    
// ----------------------------------------------------------------------------------------
// Configuration function
// ----------------------------------------------------------------------------------------

template <typename net_type>
void config_net(net_type &net, dlib::mmod_options options, std::vector<uint32_t> params)
{

    net = net_type(options);

    dlib::layer<net_type::num_layers - 1>(net).set_pyramid_outer_padding(8);
    dlib::layer<net_type::num_layers - 1>(net).set_pyramid_padding(8);

}   // end of config_net

// ----------------------------------------------------------------------------------------
//  GORGON Functions
// ----------------------------------------------------------------------------------------


//gorgon_capture<1> gc_01(256,128,3,3);
//gorgon_capture<5> gc_02(128,128,3,3);
//gorgon_capture<8> gc_03(128,192,3,3);
//gorgon_capture<14> gc_04(128,128,3,3);
//gorgon_capture<17> gc_05(128,320,3,3);
//gorgon_capture<23> gc_06(256,256,3,3);
//gorgon_capture<26> gc_07(256,576,3,3);
//gorgon_capture<32> gc_08(512,512,3,3);
//gorgon_capture<35> gc_09(512,64,3,3);
//gorgon_capture<40> gc_10(512,512,3,3);
//gorgon_capture<43> gc_11(512,64,3,3);
//gorgon_capture<48> gc_12(256,256,3,3);
//gorgon_capture<51> gc_13(256,64,3,3);
//gorgon_capture<56> gc_14(128,128,3,3);
//gorgon_capture<59> gc_15(128,img_depth,3,3);

void init_gorgon(std::string save_location)
{
    //gc_01.init((save_location + "l01"));
    //gc_02.init((save_location + "l05"));
    //gc_03.init((save_location + "l08"));
    //gc_04.init((save_location + "l14"));
    //gc_05.init((save_location + "l17"));
    //gc_06.init((save_location + "l23"));
    //gc_07.init((save_location + "l26"));
    //gc_08.init((save_location + "l32"));
    //gc_09.init((save_location + "l35"));
    //gc_10.init((save_location + "l40"));
    //gc_11.init((save_location + "l43"));
    //gc_12.init((save_location + "l48"));
    //gc_13.init((save_location + "l51"));
    //gc_14.init((save_location + "l56"));
    //gc_15.init((save_location + "l59"));

}

template<typename net_type>
void save_gorgon(net_type &dfd_net, uint64_t one_step_calls)
{
    //gc_01.save_params(dfd_net, one_step_calls);
    //gc_02.save_params(dfd_net, one_step_calls);
    //gc_03.save_params(dfd_net, one_step_calls);
    //gc_04.save_params(dfd_net, one_step_calls);
    //gc_05.save_params(dfd_net, one_step_calls);
    //gc_06.save_params(dfd_net, one_step_calls);
    //gc_07.save_params(dfd_net, one_step_calls);
    //gc_08.save_params(dfd_net, one_step_calls);
    //gc_09.save_params(dfd_net, one_step_calls);
    //gc_10.save_params(dfd_net, one_step_calls);
    //gc_11.save_params(dfd_net, one_step_calls);
    //gc_12.save_params(dfd_net, one_step_calls);
    //gc_13.save_params(dfd_net, one_step_calls);
    //gc_14.save_params(dfd_net, one_step_calls);
    //gc_15.save_params(dfd_net, one_step_calls);
}


void close_gorgon(void)
{
    //gc_01.close_stream();
    //gc_02.close_stream();
    //gc_03.close_stream();
    //gc_04.close_stream();
    //gc_05.close_stream();
    //gc_06.close_stream();
    //gc_07.close_stream();
    //gc_08.close_stream();
    //gc_09.close_stream();
    //gc_10.close_stream();
    //gc_11.close_stream();
    //gc_12.close_stream();
    //gc_13.close_stream();
    //gc_14.close_stream();
    //gc_15.close_stream();
}


#endif //NET_DEFINITION_H
