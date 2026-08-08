#version 450

layout(location = 0) flat in uint inEntityId;
layout(location = 0) out uint outEntityId;

void main()
{
    outEntityId = 123u;
}