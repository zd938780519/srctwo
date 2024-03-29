<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: google/protobuf/descriptor.proto

namespace Google\Protobuf\Internal;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\GPBWire;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\InputStream;
use Google\Protobuf\Internal\GPBUtil;

/**
 * Describes a method of a service.
 *
 * Generated from protobuf message <code>google.protobuf.MethodDescriptorProto</code>
 */
class MethodDescriptorProto extends \Google\Protobuf\Internal\Message
{
    /**
     * Generated from protobuf field <code>optional string name = 1;</code>
     */
    private $name = '';
    private $has_name = false;
    /**
     * Input and output type names.  These are resolved in the same way as
     * FieldDescriptorProto.type_name, but must refer to a message type.
     *
     * Generated from protobuf field <code>optional string input_type = 2;</code>
     */
    private $input_type = '';
    private $has_input_type = false;
    /**
     * Generated from protobuf field <code>optional string output_type = 3;</code>
     */
    private $output_type = '';
    private $has_output_type = false;
    /**
     * Generated from protobuf field <code>optional .google.protobuf.MethodOptions options = 4;</code>
     */
    private $options = null;
    private $has_options = false;
    /**
     * Identifies if client streams multiple client messages
     *
     * Generated from protobuf field <code>optional bool client_streaming = 5 [default = false];</code>
     */
    private $client_streaming = false;
    private $has_client_streaming = false;
    /**
     * Identifies if server streams multiple server messages
     *
     * Generated from protobuf field <code>optional bool server_streaming = 6 [default = false];</code>
     */
    private $server_streaming = false;
    private $has_server_streaming = false;

    public function __construct() {
        \GPBMetadata\Google\Protobuf\Internal\Descriptor::initOnce();
        parent::__construct();
    }

    /**
     * Generated from protobuf field <code>optional string name = 1;</code>
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * Generated from protobuf field <code>optional string name = 1;</code>
     * @param string $var
     * @return $this
     */
    public function setName($var)
    {
        GPBUtil::checkString($var, True);
        $this->name = $var;
        $this->has_name = true;

        return $this;
    }

    public function hasName()
    {
        return $this->has_name;
    }

    /**
     * Input and output type names.  These are resolved in the same way as
     * FieldDescriptorProto.type_name, but must refer to a message type.
     *
     * Generated from protobuf field <code>optional string input_type = 2;</code>
     * @return string
     */
    public function getInputType()
    {
        return $this->input_type;
    }

    /**
     * Input and output type names.  These are resolved in the same way as
     * FieldDescriptorProto.type_name, but must refer to a message type.
     *
     * Generated from protobuf field <code>optional string input_type = 2;</code>
     * @param string $var
     * @return $this
     */
    public function setInputType($var)
    {
        GPBUtil::checkString($var, True);
        $this->input_type = $var;
        $this->has_input_type = true;

        return $this;
    }

    public function hasInputType()
    {
        return $this->has_input_type;
    }

    /**
     * Generated from protobuf field <code>optional string output_type = 3;</code>
     * @return string
     */
    public function getOutputType()
    {
        return $this->output_type;
    }

    /**
     * Generated from protobuf field <code>optional string output_type = 3;</code>
     * @param string $var
     * @return $this
     */
    public function setOutputType($var)
    {
        GPBUtil::checkString($var, True);
        $this->output_type = $var;
        $this->has_output_type = true;

        return $this;
    }

    public function hasOutputType()
    {
        return $this->has_output_type;
    }

    /**
     * Generated from protobuf field <code>optional .google.protobuf.MethodOptions options = 4;</code>
     * @return \Google\Protobuf\Internal\MethodOptions
     */
    public function getOptions()
    {
        return $this->options;
    }

    /**
     * Generated from protobuf field <code>optional .google.protobuf.MethodOptions options = 4;</code>
     * @param \Google\Protobuf\Internal\MethodOptions $var
     * @return $this
     */
    public function setOptions($var)
    {
        GPBUtil::checkMessage($var, \Google\Protobuf\Internal\MethodOptions::class);
        $this->options = $var;
        $this->has_options = true;

        return $this;
    }

    public function hasOptions()
    {
        return $this->has_options;
    }

    /**
     * Identifies if client streams multiple client messages
     *
     * Generated from protobuf field <code>optional bool client_streaming = 5 [default = false];</code>
     * @return bool
     */
    public function getClientStreaming()
    {
        return $this->client_streaming;
    }

    /**
     * Identifies if client streams multiple client messages
     *
     * Generated from protobuf field <code>optional bool client_streaming = 5 [default = false];</code>
     * @param bool $var
     * @return $this
     */
    public function setClientStreaming($var)
    {
        GPBUtil::checkBool($var);
        $this->client_streaming = $var;
        $this->has_client_streaming = true;

        return $this;
    }

    public function hasClientStreaming()
    {
        return $this->has_client_streaming;
    }

    /**
     * Identifies if server streams multiple server messages
     *
     * Generated from protobuf field <code>optional bool server_streaming = 6 [default = false];</code>
     * @return bool
     */
    public function getServerStreaming()
    {
        return $this->server_streaming;
    }

    /**
     * Identifies if server streams multiple server messages
     *
     * Generated from protobuf field <code>optional bool server_streaming = 6 [default = false];</code>
     * @param bool $var
     * @return $this
     */
    public function setServerStreaming($var)
    {
        GPBUtil::checkBool($var);
        $this->server_streaming = $var;
        $this->has_server_streaming = true;

        return $this;
    }

    public function hasServerStreaming()
    {
        return $this->has_server_streaming;
    }

}

