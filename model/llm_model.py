import torch
import torch.nn as nn

class TransformerBlock(nn.Module):
    def __init__(self,embed_size,heads,dropout,forward_expansion):
        super(TransformerBlock, self).__init__()
        # 多头自注意力机制
        # embed_size:词嵌入维度
        # heads:头数
        self.attention = nn.MultiheadAttention(embed_size,heads)
        self.norm1 = nn.LayerNorm(embed_size)
        # 前馈神经网络
        self.feed_forward = nn.Sequential(
            nn.Linear(embed_size , forward_expansion * embed_size),
            nn.ReLU(),
            nn.Linear(forward_expansion * embed_size , embed_size)
        )
        self.dropout1 = nn.Dropout(dropout)
        self.dropout2 = nn.Dropout(dropout)
    
    def forward(self , x):
        attention = self.attention(x , x , x)[0]
        x = self.dropout1(attention + x)
        forward = self.feed_forward(x)
        x = self.dropout2(forward + x)
        return x

class LLM(nn.Module):
    def __init__(self , embed_size , heads , num_layers , forward_expansion , 
                 dropout , max_length , vocab_size): 
        """
        LLM language model
        """
        super(LLM , self).__init__()
        self.embedding = nn.Embedding(vocab_size , embed_size)
        # 位置编码
        self.position_embedding = nn.Embedding(max_length , embed_size)
        # transformer block
        self.transformer_blocks = nn.ModuleList(
            [TransformerBlock(embed_size , heads  , dropout,
                              forward_expansion) for _ in range(num_layers)]
        )
        # 输出层
        self.fc_out = nn.Linear(embed_size , vocab_size)
        self.dropout = nn.Dropout(dropout)

    def forward(self , x):
        N , seq_length = x.shape
        positions = torch.arange(0 , seq_length).expand(N , seq_length).to(x.device)
        out = self.embedding(x)+self.position_embedding(positions)
        for transformer in self.transformer_blocks:
            out = transformer(out)
        out = self.fc_out(out)
        return out


