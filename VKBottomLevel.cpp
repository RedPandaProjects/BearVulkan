#include "VKPCH.h"
size_t BottomLevelCounter = 0;
#ifdef RTX
VKBottomLevel::VKBottomLevel(const BearBottomLevelDescription& desc):AccelerationStructure(VK_NULL_HANDLE), ResultBufferMemory(VK_NULL_HANDLE)
{
	BottomLevelCounter++;

	BearVector<VkGeometryNV>GeometryDescs;
	for (const  BearBottomLevelDescription::GeometryDescription& i : desc.GeometryDescriptions)
	{
		VkGeometryNV Gometry = {};
		Gometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
		Gometry.pNext = VK_NULL_HANDLE;
		Gometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
		Gometry.geometry.triangles.pNext = VK_NULL_HANDLE;
		Gometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
		Gometry.geometry.aabbs.pNext = VK_NULL_HANDLE;
		switch (i.Type)
		{
		case BearRaytracingGeometryType::Triangles:
			Gometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
			break;
		case BearRaytracingGeometryType::ProceduralPrimitiveAABBS:
			Gometry.geometryType = VK_GEOMETRY_TYPE_AABBS_NV;
			break;
		default:
			BEAR_CHECK(false);
		}
		{
			Gometry.flags = 0;
			if (i.Flags.test((uint32)BearRaytracingGeometryFlags::Opaque))
			{
				Gometry.flags |= VK_GEOMETRY_OPAQUE_BIT_NV;
			}
			if (i.Flags.test((uint32)BearRaytracingGeometryFlags::NoDuplicateAnyhitInvocation))
			{
				Gometry.flags |= VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_NV;
			}
		}
		if (Gometry.geometryType == VK_GEOMETRY_TYPE_TRIANGLES_NV)
		{

			BEAR_CHECK(!i.Triangles.VertexBuffer.empty());
			Gometry.geometry.triangles.transformData = VK_NULL_HANDLE;
			{
				auto VertexBuffer = static_cast<const VKVertexBuffer*>(i.Triangles.VertexBuffer.get());
				BEAR_CHECK(VertexBuffer->Size);
				BEAR_CHECK(VertexBuffer->VertexDescription.stride);
				Gometry.geometry.triangles.vertexData = VertexBuffer->Buffer;
				Gometry.geometry.triangles.vertexStride = VertexBuffer->VertexDescription.stride;
				Gometry.geometry.triangles.vertexOffset = i.Triangles.VertexOffset;
				Gometry.geometry.triangles.vertexCount = i.Triangles.VertexCount;
				switch (i.Triangles.VertexFormat)
				{
				case VF_R16G16_FLOAT:
					Gometry.geometry.triangles.vertexFormat = VK_FORMAT_R16G16_SFLOAT;
					break;
				case VF_R16G16B16A16_FLOAT:
					Gometry.geometry.triangles.vertexFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
					break;
				case VF_R32G32_FLOAT:
					Gometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32_SFLOAT;
					break;
				case VF_R32G32B32_FLOAT:
					Gometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
					break;
				default:
					BEAR_CHECK(false);
					break;
				}
				BEAR_CHECK(i.Triangles.VertexCount >= VertexBuffer->Size / VertexBuffer->VertexDescription.stride);
				
				
			}
			if (i.Triangles.IndexBuffer.empty())
			{
				Gometry.geometry.triangles.indexType = VK_INDEX_TYPE_NONE_NV;
			}
			else
			{
				Gometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
				auto IndexBuffer = static_cast<const VKIndexBuffer*>(i.Triangles.IndexBuffer.get());
				BEAR_CHECK(IndexBuffer->Size);
				Gometry.geometry.triangles.indexData = IndexBuffer->Buffer;
				Gometry.geometry.triangles.indexOffset = i.Triangles.IndexOffset;
				Gometry.geometry.triangles.indexCount = i.Triangles.IndexCount;
			}
		}
		else
		{
			BEAR_CHECK(!i.AABB.Buffer.empty());
			auto Buffer = static_cast<const VKStructuredBuffer*>(i.AABB.Buffer.get());
			Gometry.geometry.aabbs.aabbData = Buffer->Buffer;
			Gometry.geometry.aabbs.numAABBs = i.AABB.Count;
			Gometry.geometry.aabbs.offset = i.AABB.Offset;
			Gometry.geometry.aabbs.stride = i.AABB.Stride;
		}
		GeometryDescs.emplace_back(Gometry);
	}
	VkAccelerationStructureInfoNV AccelerationStructureInfo = {};

	{
	
		AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		AccelerationStructureInfo.pNext = VK_NULL_HANDLE;
		AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
		AccelerationStructureInfo.flags = 0;
		AccelerationStructureInfo.instanceCount = VK_NULL_HANDLE;
		AccelerationStructureInfo.geometryCount = static_cast<uint32_t>(GeometryDescs.size());
		AccelerationStructureInfo.pGeometries = GeometryDescs.data();

		VkAccelerationStructureCreateInfoNV AccelerationStructureCreateInfo = {};
		AccelerationStructureCreateInfo.sType =	VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
		AccelerationStructureCreateInfo.pNext = VK_NULL_HANDLE;
		AccelerationStructureCreateInfo.info = AccelerationStructureInfo;
		AccelerationStructureCreateInfo.compactedSize = 0;

		VkAccelerationStructureNV accelerationStructure;
		V_CHK(vkCreateAccelerationStructureNV(Factory->Device, &AccelerationStructureCreateInfo, nullptr,&AccelerationStructure));
	}
	bsize ResultSizeInBytes = 0;
	bsize ResultMemoryTypeBits = 0;
	bsize ScratchSizeInBytes = 0;
	{
		VkAccelerationStructureMemoryRequirementsInfoNV MemoryRequirementsInfo;
		MemoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		MemoryRequirementsInfo.pNext = VK_NULL_HANDLE;
		MemoryRequirementsInfo.accelerationStructure = AccelerationStructure;
		MemoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;

		VkMemoryRequirements2 MemoryRequirements = {};
		MemoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vkGetAccelerationStructureMemoryRequirementsNV(Factory->Device, &MemoryRequirementsInfo,&MemoryRequirements);

		ResultSizeInBytes = MemoryRequirements.memoryRequirements.size;
		ResultMemoryTypeBits = MemoryRequirements.memoryRequirements.memoryTypeBits;
		// Store the memory requirements for use during build/update
		MemoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;
		vkGetAccelerationStructureMemoryRequirementsNV(Factory->Device, &MemoryRequirementsInfo,&MemoryRequirements);

		ScratchSizeInBytes = MemoryRequirements.memoryRequirements.size;
		/*MemoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_NV;
		vkGetAccelerationStructureMemoryRequirementsNV(Factory->Device, &MemoryRequirementsInfo,&MemoryRequirements);*/

	}
	VkBuffer ScratchBuffer;
	VkDeviceMemory ScratchBufferMemory;
	CreateBuffer(Factory->PhysicalDevice, Factory->Device, ScratchSizeInBytes, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ScratchBuffer, ScratchBufferMemory);
	
	

	{
		VkMemoryAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = ResultSizeInBytes;
		AllocInfo.memoryTypeIndex = FindMemoryType(Factory->PhysicalDevice, ResultMemoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		V_CHK(vkAllocateMemory(Factory->Device, &AllocInfo, nullptr, &ResultBufferMemory));
	}

	VkBindAccelerationStructureMemoryInfoNV bindInfo;
	bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
	bindInfo.pNext = nullptr;
	bindInfo.accelerationStructure = AccelerationStructure;
	bindInfo.memory = ResultBufferMemory;
	bindInfo.memoryOffset = 0;
	bindInfo.deviceIndexCount = 0;
	bindInfo.pDeviceIndices = nullptr;

	V_CHK(vkBindAccelerationStructureMemoryNV(Factory->Device, 1, &bindInfo));

	Factory->LockCommandBuffer();

	vkCmdBuildAccelerationStructureNV(Factory->CommandBuffer, &AccelerationStructureInfo, VK_NULL_HANDLE, 0, false, AccelerationStructure, VK_NULL_HANDLE, ScratchBuffer, 0);
	VkMemoryBarrier MemoryBarrier;
	MemoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	MemoryBarrier.pNext = nullptr;
	MemoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
	MemoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

	vkCmdPipelineBarrier(Factory->CommandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &MemoryBarrier,0, nullptr, 0, nullptr);
	Factory->UnlockCommandBuffer();


	V_CHK(vkGetAccelerationStructureHandleNV(Factory->Device,AccelerationStructure, sizeof(uint64_t),&AccelerationStructureHandle));
	vkDestroyBuffer(Factory->Device, ScratchBuffer, VK_NULL_HANDLE);
	vkFreeMemory(Factory->Device, ScratchBufferMemory, 0);
}

VKBottomLevel::~VKBottomLevel()
{
	vkDestroyAccelerationStructureNV(Factory->Device, AccelerationStructure, VK_NULL_HANDLE);
	vkFreeMemory(Factory->Device, ResultBufferMemory, VK_NULL_HANDLE);
	BottomLevelCounter--;
}

void* VKBottomLevel::QueryInterface(int Type)
{
	return nullptr;
}
#endif