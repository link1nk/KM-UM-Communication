#include <ntddk.h>
#include "ReadWriteBufferedIO-Type.h"

NTSTATUS CompleteRequest(_In_ PIRP Irp, _In_opt_ NTSTATUS Status = STATUS_SUCCESS, _In_opt_ ULONG Information = 0)
{
	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = Information;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}

VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLink = RTL_CONSTANT_STRING(L"\\??\\ReadWriteBufferedIO");

	IoDeleteSymbolicLink(&SymbolicLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS CreateCloseRequest(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	return CompleteRequest(Irp);
}

NTSTATUS WriteRequest(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS Status = STATUS_SUCCESS;
	ULONG Information = 0;

	auto IrpSp = IoGetCurrentIrpStackLocation(Irp);

	do
	{
		if (IrpSp->Parameters.Write.Length < sizeof(WRITE_REQUEST))
		{
			Status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		auto WriteRequest = reinterpret_cast<PWRITE_REQUEST>(Irp->AssociatedIrp.SystemBuffer);

		DbgPrint("This is a Write Request | %s\n", WriteRequest->InputBuffer);

		Information = sizeof(WRITE_REQUEST);

	} while (false);

	return CompleteRequest(Irp, Status, Information);
}

NTSTATUS ReadRequest(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS Status = STATUS_SUCCESS;
	ULONG Information = 0;

	auto IrpSp = IoGetCurrentIrpStackLocation(Irp);

	do
	{
		if (IrpSp->Parameters.Read.Length < sizeof(READ_REQUEST))
		{
			Status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		auto ReadRequest = reinterpret_cast<PREAD_REQUEST>(Irp->AssociatedIrp.SystemBuffer);

		PCCHAR Message = "This message was copy from driver!";

		strcpy_s(ReadRequest->OutputBuffer, sizeof(ReadRequest->OutputBuffer), Message);

		Information = sizeof(Message);

	} while (false);

	return CompleteRequest(Irp, Status, Information);
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCloseRequest;
	DriverObject->MajorFunction[IRP_MJ_CLOSE]  = CreateCloseRequest;
	DriverObject->MajorFunction[IRP_MJ_WRITE]  = WriteRequest;
	DriverObject->MajorFunction[IRP_MJ_READ]   = ReadRequest;

	UNICODE_STRING DeviceName   = RTL_CONSTANT_STRING(L"\\Device\\ReadWriteBufferedIO");
	UNICODE_STRING SymbolicLink = RTL_CONSTANT_STRING(L"\\??\\ReadWriteBufferedIO");

	PDEVICE_OBJECT DeviceObject;

	NTSTATUS Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Erro ao criar o Device Object\n");
		return Status;
	}

	DeviceObject->Flags |= DO_BUFFERED_IO;

	Status = IoCreateSymbolicLink(&SymbolicLink, &DeviceName);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Erro ao criar o Symbolic Link\n");
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	return Status;
}