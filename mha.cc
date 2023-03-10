#include <iostream>
#include <fstream>
#include "eigenDNN.h"

using namespace std;

void test_eidnnLinearForward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 2> q_weight(hidden_size, hidden_size); 
  Tensor<float, 3> q_in(batch_size, seq_len, hidden_size);
  Tensor<float, 3> q_out(batch_size, seq_len, hidden_size);

  q_out.setConstant(0);
  q_weight.setConstant(1);
  q_in.setConstant(1);
  
  eigenDNN::eidnnLinearForward(handle, q_in, q_weight, q_out);
  cout << "q_out: " << endl << q_out << endl;
}

void test_eidnnLinearBackward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 3> q_out_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> q_in(batch_size, seq_len, hidden_size);
  Tensor<float, 2> q_weight(hidden_size, hidden_size); 

  Tensor<float, 3> q_in_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 2> q_weight_grad(hidden_size, hidden_size); 

  q_in_grad.setConstant(0);
  q_weight_grad.setConstant(0);

  q_out_grad.setConstant(1);
  q_in.setConstant(1);
  q_weight.setConstant(1);
  
  eigenDNN::eidnnLinearBackward(handle, q_out_grad, q_in, q_weight, q_in_grad, q_weight_grad);
  cout << "q_in_grad: " << endl << q_in_grad << endl;
  cout << "q_weight_grad: " << endl << q_weight_grad << endl;
}


void test_eidnnSoftmaxForward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 4> s_in(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> s_out(batch_size, n_heads, seq_len, seq_len);

  s_out.setConstant(0);
  s_in.setConstant(1);
  
  eigenDNN::eidnnSoftmaxForward(handle, eigenDNN::eidnnSoftmaxAlgorithm_t::EIDNN_SOFTMAX_ACCURATE, eigenDNN::eidnnSoftmaxMode_t::EIDNN_SOFTMAX_MODE_INSTANCE, s_in, s_out);
  cout << "s_in: " << endl << s_in << endl;
  cout << "s_out: " << endl << s_out << endl;
}

void test_eidnnSoftmaxBackward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 4> s_in_grad(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> s_out(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> s_out_grad(batch_size, n_heads, seq_len, seq_len);

  s_in_grad.setConstant(0);

  s_out_grad.setConstant(0.1);
  s_out.setConstant(1);
  
  eigenDNN::eidnnSoftmaxBackward(handle, eigenDNN::eidnnSoftmaxAlgorithm_t::EIDNN_SOFTMAX_ACCURATE, eigenDNN::eidnnSoftmaxMode_t::EIDNN_SOFTMAX_MODE_INSTANCE, s_out, s_out_grad, s_in_grad);
  cout << "s_out_grad: " << endl << s_out_grad << endl;
  cout << "s_out: " << endl << s_out << endl;
  cout << "s_in_grad: " << endl << s_in_grad << endl;
  
}


void test_eidnnDropoutForwardBackward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 4> do_in(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> do_out(batch_size, n_heads, seq_len, seq_len);

  do_out.setConstant(0);
  do_in.setConstant(1);

  eigenDNN::eidnnDropoutDescriptor_t dropoutDesc = make_tuple(dropout_rate,saved_states,0,2023);
  
  eigenDNN::eidnnDropoutForward(handle, dropoutDesc, do_in, do_out);
  cout << "do_in: " << endl << do_in << endl;
  cout << "do_out: " << endl << do_out << endl;


  Tensor<float, 4> do_in_grad(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> do_out_grad(batch_size, n_heads, seq_len, seq_len);

  do_in_grad.setConstant(0);
  do_out_grad.setConstant(1);

  eigenDNN::eidnnDropoutBackward(handle, dropoutDesc, do_out_grad, do_in_grad);
  cout << "do_out_grad: " << endl << do_out_grad << endl;
  cout << "do_in_grad: " << endl << do_in_grad << endl;
}


void test_eidnnMatMulForwardBackward(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 4> a(batch_size, n_heads, seq_len, head_size); 
  Tensor<float, 4> b(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> c(batch_size, n_heads, seq_len, seq_len);

  
  Tensor<float, 4> a_grad(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> b_grad(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> c_grad(batch_size, n_heads, seq_len, seq_len); 


  c.setConstant(0);
  a.setConstant(1);
  b.setConstant(1);

  a_grad.setConstant(0);
  b_grad.setConstant(0);
  c_grad.setConstant(1);
  
  eigenDNN::eidnnStridedBatchGemmForward(handle, 1, 0, false, true, false, a, b, c);
  cout << "a: " << endl << a << endl;
  cout << "b: " << endl << b << endl;
  cout << "c: " << endl << c << endl;

  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, true, false, false, b, c_grad, a_grad);
  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, false, false, true, a, c_grad, b_grad);
  cout << "a_grad: " << endl << a_grad << endl;
  cout << "b_grad: " << endl << b_grad << endl;
  cout << "c_grad: " << endl << c_grad << endl;
}

void test_eidnnMSELoss(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 3> output(batch_size, seq_len, hidden_size);
  Tensor<float, 3> target(batch_size, seq_len, hidden_size);
  Tensor<float, 0> loss;
  Tensor<float, 3> d_loss(batch_size, seq_len, hidden_size);

  output.setConstant(1);
  target.setConstant(0.5);
  loss.setConstant(0);
  d_loss.setConstant(0);

  eigenDNN::eidnnMSELoss(handle, output, target, loss, d_loss);
  cout << "output: " << endl << output << endl;
  cout << "target: " << endl << target << endl;
  cout << "loss: " << endl << loss << endl;
  cout << "d_loss: " << endl << d_loss << endl;
}




void test_mha(unsigned batch_size,unsigned n_heads,unsigned seq_len,unsigned head_size,unsigned hidden_size,float dropout_rate, void* saved_states){
  using namespace Eigen;
  eigenDNN::eidnnHandle_t handle;

  Tensor<float, 2> q_weight(hidden_size, hidden_size); 
  Tensor<float, 2> k_weight(hidden_size, hidden_size); 
  Tensor<float, 2> v_weight(hidden_size, hidden_size); 
  Tensor<float, 2> o_weight(hidden_size, hidden_size); 
  
  Tensor<float, 3> q_in(batch_size, seq_len, hidden_size);
  Tensor<float, 3> k_in(batch_size, seq_len, hidden_size);
  Tensor<float, 3> v_in(batch_size, seq_len, hidden_size);
  Tensor<float, 3> o_in(batch_size, seq_len, hidden_size);
  
  Tensor<float, 3> q_out(batch_size, seq_len, hidden_size);
  Tensor<float, 3> k_out(batch_size, seq_len, hidden_size);
  Tensor<float, 3> v_out(batch_size, seq_len, hidden_size);
  Tensor<float, 3> o_out(batch_size, seq_len, hidden_size);
  
  Tensor<float, 4> q(batch_size, n_heads, seq_len, head_size); 
  Tensor<float, 4> k(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> v(batch_size, n_heads, seq_len, head_size);
  
  Tensor<float, 4> s(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> p(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> o(batch_size, n_heads, seq_len, head_size);
  
  Tensor<float, 3> q_out_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> k_out_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> v_out_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> o_out_grad(batch_size, seq_len, hidden_size);

  Tensor<float, 3> q_in_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> k_in_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> v_in_grad(batch_size, seq_len, hidden_size);
  Tensor<float, 3> o_in_grad(batch_size, seq_len, hidden_size);
  
  Tensor<float, 2> q_weight_grad(hidden_size, hidden_size); 
  Tensor<float, 2> k_weight_grad(hidden_size, hidden_size); 
  Tensor<float, 2> v_weight_grad(hidden_size, hidden_size); 
  Tensor<float, 2> o_weight_grad(hidden_size, hidden_size); 
  
  Tensor<float, 4> q_grad(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> k_grad(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> v_grad(batch_size, n_heads, seq_len, head_size);
  Tensor<float, 4> s_grad(batch_size, n_heads, seq_len, seq_len); 
  Tensor<float, 4> p_grad(batch_size, n_heads, seq_len, seq_len);
  Tensor<float, 4> o_grad(batch_size, n_heads, seq_len, head_size); 
  
  Tensor<float, 3> target(batch_size, seq_len, hidden_size);
  Tensor<float, 0> loss;
  Tensor<float, 3> d_loss(batch_size, seq_len, hidden_size);

  q_weight.setRandom();
  k_weight.setRandom();
  v_weight.setRandom();
  o_weight.setRandom();

  q_in.setConstant(1);
  k_in.setConstant(1);
  v_in.setConstant(1);

  target.setConstant(0.5);
  
  
  // Linear Layer for Q, K and V, forward
  eigenDNN::eidnnLinearForward(handle, q_in, q_weight, q_out);
  eigenDNN::eidnnLinearForward(handle, k_in, k_weight, k_out);
  eigenDNN::eidnnLinearForward(handle, v_in, v_weight, v_out);
  
  // reshape Q, K and V, [batch_size, seq_len, hidden_size] -> [batch_size, n_heads, seq_len, head_size]
  TensorMap<Tensor<float, 4>> q_0123(q_out.data(), {batch_size, seq_len, n_heads, head_size});
  TensorMap<Tensor<float, 4>> k_0123(q_out.data(), {batch_size, seq_len, n_heads, head_size});
  TensorMap<Tensor<float, 4>> v_0123(q_out.data(), {batch_size, seq_len, n_heads, head_size});
  q = q_0123.shuffle(Eigen::array<int, 4>({0,2,1,3}));
  k = k_0123.shuffle(Eigen::array<int, 4>({0,2,1,3}));
  v = v_0123.shuffle(Eigen::array<int, 4>({0,2,1,3}));

  // S = Q*K^T, forward
  eigenDNN::eidnnStridedBatchGemmForward(handle, 1, 0, false, true, false, q, k, s);  
  
  // P = softmax(S), forward
  eigenDNN::eidnnSoftmaxForward(handle, eigenDNN::eidnnSoftmaxAlgorithm_t::EIDNN_SOFTMAX_ACCURATE, eigenDNN::eidnnSoftmaxMode_t::EIDNN_SOFTMAX_MODE_INSTANCE, s, p);
  
  // P = dropout(P), forward
  eigenDNN::eidnnDropoutDescriptor_t dropoutDesc = make_tuple(dropout_rate,saved_states,0,2023);
  eigenDNN::eidnnDropoutForward(handle, dropoutDesc, p, p);
  
  // O=P*V, forward
  eigenDNN::eidnnStridedBatchGemmForward(handle, 1, 0, false, false, false, p, v, o);
  
  // reshape O, [batch_size, n_heads, seq_len, head_size] -> [batch_size, seq_len, hidden_size]
  o_in = TensorMap<Tensor<float, 3>>(static_cast<Tensor<float, 4>>(o.shuffle(Eigen::array<int, 4>({0,2,1,3}))).data(), {batch_size, seq_len, hidden_size});
  
  // Linear Layer for O, forward
  eigenDNN::eidnnLinearForward(handle, o_in, o_weight, o_out);
  
  // MSE Loss
  eigenDNN::eidnnMSELoss(handle, o_out, target, loss, d_loss);
  
  // Linear Layer for O, backward
  o_out_grad = d_loss;
  eigenDNN::eidnnLinearBackward(handle, o_out_grad, o_in, o_weight, o_in_grad, o_weight_grad);
  
  // reshape O, [batch_size, seq_len, hidden_size] -> [batch_size, n_heads, seq_len, head_size]
  TensorMap<Tensor<float, 4>> o_grad_0123(q_out.data(), {batch_size, seq_len, n_heads, head_size});
  o_grad = o_grad_0123.shuffle(Eigen::array<int, 4>({0,2,1,3}));
  
  // O=P*V backward
  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, false, false, false, v, o_grad, p_grad);
  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, false, false, false, p, o_grad, v_grad);
  
  // P = dropout(P), backward
  eigenDNN::eidnnDropoutBackward(handle, dropoutDesc, p_grad, p_grad);
  
  // P = softmax(S), backward
  eigenDNN::eidnnSoftmaxBackward(handle, eigenDNN::eidnnSoftmaxAlgorithm_t::EIDNN_SOFTMAX_ACCURATE, eigenDNN::eidnnSoftmaxMode_t::EIDNN_SOFTMAX_MODE_INSTANCE, p, p_grad, s_grad);
  
  // S = Q*K^T, backward
  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, true, false, false, k, s_grad, q_grad);
  eigenDNN::eidnnStridedBatchGemmBackward(handle,  1, 0, false, false, true, q, s_grad, k_grad);
  
  // reshape Q, K and V, [batch_size, n_heads, seq_len, head_size] -> [batch_size, seq_len, hidden_size] 
  q_out_grad = TensorMap<Tensor<float, 3>>(static_cast<Tensor<float, 4>>(q_grad.shuffle(Eigen::array<int, 4>({0,2,1,3}))).data(), {batch_size, seq_len, hidden_size});
  k_out_grad = TensorMap<Tensor<float, 3>>(static_cast<Tensor<float, 4>>(q_grad.shuffle(Eigen::array<int, 4>({0,2,1,3}))).data(), {batch_size, seq_len, hidden_size});
  v_out_grad = TensorMap<Tensor<float, 3>>(static_cast<Tensor<float, 4>>(q_grad.shuffle(Eigen::array<int, 4>({0,2,1,3}))).data(), {batch_size, seq_len, hidden_size});

  // Linear Layer for Q, K and V, backward
  eigenDNN::eidnnLinearBackward(handle, q_out_grad, q_in, q_weight, q_in_grad, q_weight_grad);
  eigenDNN::eidnnLinearBackward(handle, k_out_grad, k_in, k_weight, k_in_grad, k_weight_grad);
  eigenDNN::eidnnLinearBackward(handle, v_out_grad, v_in, v_weight, v_in_grad, v_weight_grad);

  // cout << "q_weight_grad: " << endl << q_weight_grad << endl;
  // cout << "k_weight_grad: " << endl << k_weight_grad << endl;
  // cout << "v_weight_grad: " << endl << v_weight_grad << endl;
  // cout << "q_weight: " << endl << q_weight << endl;
}



int main(int argc, char* argv[]) {
  unsigned batch_size = 2;
  unsigned n_heads = 2;
  unsigned seq_len = 2;
  unsigned head_size = 2;
  unsigned hidden_size = head_size*n_heads;
  float dropout_rate = 0.1;
  void* saved_states;
  
  // test_eidnnLinearForward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnLinearBackward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnSoftmaxForward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnSoftmaxBackward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnDropoutForwardBackward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnMatMulForwardBackward(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  // test_eidnnMSELoss(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  test_mha(batch_size,n_heads,seq_len,head_size,hidden_size,dropout_rate,saved_states);
  return 0;
}
